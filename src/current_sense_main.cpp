#include "common.h"
#include <algorithm>
#include <chrono>
#include "edge_impulse_data.h"

using namespace std::chrono;

// Define the analog pins (adjust if needed)
AnalogIn sensorPinA0(A0);
AnalogIn sensorPinA1(A1);  // Sensor reference (Uref)
AnalogIn sensorPinA2(A2);  // Sensor output (Uout)
BufferedSerial pc(USBTX, USBRX, 9600);

// *******************************************************************
// Sensor and ADC configuration for LEM HLSR 10-P
// *******************************************************************
// The sensor’s sensitivity is 80 mV/A.
// The board’s ADC actually returns a 16‐bit value, but if your ADC is effectively 10‐bit,
// you can shift right by 6 bits (16 - 6 = 10). Then, with a 3 V (3000 mV) reference,
// the ADC reading spans 0–1023.
const float HALL_SENSITIVITY = 80.0f;           // in mV/A
const float VOLTAGE_REFERENCE_MV = 3000.0f;       // in millivolts (3V reference)
const int BIT_RESOLUTION = 10;                    // effective resolution after shifting
const int ITERATION = 50;                         // Number of samples for filtering

// Low-Pass Filter Constant for smoothing
const float ALPHA = 0.1f;

// *******************************************************************
// SOC and Coulomb Counting variables (unchanged)
// *******************************************************************
const float BATTERY_CAP_AH = 5.0f;
float SOC = 100.0f;           
uint64_t prevMillis = 0;
const long interval = 1000;  // 1000 ms = 1 sec
float totalCoulombs = 0.0f;

// Timer for current-sensing timing
Timer currentSenseTimer;

// Declare the global raw data buffer for the ML algorithm.
// This buffer is expected to have a size of EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE (60 floats, for example).
float raw_data_buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

// Function prototypes
float HallEffectSensor();
float CalcMedian(int* arr_sv, size_t len_arr_sv);
float ApplyLowPassFilter(float current);

// Main function for current sensing
void current_sense_main(float *axis_values) {
    // Start the timer on first call
    if (currentSenseTimer.elapsed_time().count() == 0) {
        currentSenseTimer.start();
        printf("10A Current Shunt Sensor\n");
    }
    
    auto currentMillis = duration_cast<milliseconds>(Kernel::Clock::now().time_since_epoch()).count();

    // Check if the interval has elapsed
    if (currentMillis - prevMillis >= interval) {
        prevMillis = currentMillis;

        float current_a = HallEffectSensor();
        current_a = ApplyLowPassFilter(current_a);  // Smooth out high-frequency noise

        // Coulomb counting (charge in A*s)
        float charge = current_a * (interval / 1000.0f);
        totalCoulombs += charge;

        // Calculate SOC change (using coulomb counting)
        float socChange = (charge / 3600.0f) / BATTERY_CAP_AH * 100.0f;
        SOC += socChange;
        if (SOC > 100.0f) {
            SOC = 100.0f;
        } else if (SOC < 0.0f) {
            SOC = 0.0f;
        }

        // Instead of assigning to the EKF variable:
        // BatterySOCEstimation_rev_U.In1 = current;
        // Assign the current sensor measurement to the ML algorithm's raw data buffer.
        // For example, if the ML model expects channels in the order:
        // voltage (index 0), current (index 1), ah (index 2), power (index 3), battery_temp (index 4), brand (index 5),
        // then we store the current measurement at index 1.
        axis_values[1] = current_a;
    }
}

// Calculate the median of an array of integers (robust to outliers)
float CalcMedian(int* arr_sv, size_t len_arr_sv) {
    std::sort(arr_sv, arr_sv + len_arr_sv);
    if (len_arr_sv % 2 != 0) {
        return static_cast<float>(arr_sv[len_arr_sv / 2]);
    }
    return static_cast<float>((arr_sv[(len_arr_sv - 1) / 2] + arr_sv[len_arr_sv / 2]) / 2.0f);
}

// Apply a simple low-pass (exponential smoothing) filter
float ApplyLowPassFilter(float current) {
    static float filteredCurrent = 0.0f;
    filteredCurrent = (ALPHA * current) + ((1.0f - ALPHA) * filteredCurrent);
    return filteredCurrent;
}

// Read and process Hall-effect sensor data using a median filter for noise reduction
float HallEffectSensor() {
    int RefSensorValues[ITERATION];
    int OutSensorValues[ITERATION];
    float vref, vout;
    float ref_voltage_mV, out_voltage_mV;
    float voltage_diff_mV;
    float current;

    // Sample the ADC multiple times (shift 16-bit result down by 6 bits to get 10-bit value)
    for (int i = 0; i < ITERATION; i++) {
        RefSensorValues[i] = sensorPinA1.read_u16() >> 6; // Uref reading
        OutSensorValues[i] = sensorPinA2.read_u16() >> 6;   // Uout reading
        ThisThread::sleep_for(1ms);
    }

    // Compute median values from the samples
    vref = CalcMedian(RefSensorValues, ITERATION);
    vout = CalcMedian(OutSensorValues, ITERATION);

    // Convert the 10-bit ADC reading (0..1023) to millivolts using a 3000 mV reference
    ref_voltage_mV = (vref / 1023.0f) * VOLTAGE_REFERENCE_MV;
    out_voltage_mV = (vout / 1023.0f) * VOLTAGE_REFERENCE_MV;

    // For the LEM sensor, Uout = Uref + (Sensitivity × Current)
    voltage_diff_mV = out_voltage_mV - ref_voltage_mV;

    // Calculate the current in amperes (mV divided by mV/A)
    current = voltage_diff_mV / HALL_SENSITIVITY;

    // Print the current reading
    // printf("Current: %.3f A\n", current);

    // Update the EKF input with the measured current
    // BatterySOCEstimation_rev_U.In1 = current;

    return current;
}
