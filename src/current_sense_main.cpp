#include "mbed.h"
#include <algorithm> // For sorting the array in the median filter

extern "C" {
    #include "BatterySOCEstimation_rev.h"
}

// NRF52-DK uses specific pins. Replace with the correct pins for your MCU.
AnalogIn sensorPinA0(A0); // Define the analog pin (Replace with actual pin names if needed)
AnalogIn sensorPinA1(A1);
AnalogIn sensorPinA2(A2);
BufferedSerial pc(USBTX, USBRX, 9600); // Use BufferedSerial for serial communication

const float SHUNT_CURRENT_A = 10.00;
const float SHUNT_VOLTAGE_MV = 100.0;
const float HALL_SENSITIVITY = 80.0;   // Sensitivity of the Hall sensor in mV/A
const float OP_AMP_GAIN = 4.9819;      // Gain applied by the operational amplifier

const int ITERATION = 50;  // Increased number of samples for better filtering
const float VOLTAGE_REFERENCE_V = 3.00; // 3V reference CHECK THIS
const int BIT_RESOLUTION = 16;  // 16-bit ADC resolution CHECK THIS

// Low-Pass Filter Constant
const float ALPHA = 0.1;  // Alpha value for exponential smoothing

// SOC Global Variables
const float BATTERY_CAP_AH = 5.0;
float SOC = 100.0; // Initial SOC
unsigned long prevMillis = 0;
const long interval = 1000; // 1000ms = 1 sec
float totalCoulombs = 0.0;

// Timer object to handle periodic calls
Timer currentSenseTimer;

// Function prototypes
void printCurrent();
float HallEffectSensor();
float CalcMedian(int* arr_sv, size_t len_arr_sv);
float ApplyLowPassFilter(float current);

// Main function for current sensing
void current_sense_main() {
    // Start the timer if it hasn't been started yet
    if (currentSenseTimer.elapsed_time().count() == 0) {
        currentSenseTimer.start();
        printf("Hello from current sensor!\n");
        pc.write("10A Current Shunt Sensor\n", 26);
    }

    unsigned long currentMillis = Kernel::get_ms_count(); // Use Mbed's time tracking

    // Check if the timer has passed the desired interval
    if (currentMillis - prevMillis >= interval) {
        prevMillis = currentMillis;

        float current_a = HallEffectSensor();
        current_a = ApplyLowPassFilter(current_a); // Apply low-pass filter to smooth out the noise

        // **** Update the EKF input with the measured current ****
        BatterySOCEstimation_rev_U.In1 = current_a;

        // Calculate the charge (A*s)
        float charge = current_a * (interval / 1000.0);
        totalCoulombs += charge;

        float socChange = (charge / 3600.0) / BATTERY_CAP_AH * 100;

        // Updating the SOC
        SOC += socChange;

        if (SOC > 100.0) {
            SOC = 100.0;
        } else if (SOC < 0.0) {
            SOC = 0.0;
        }

        // Print the SOC
        char soc_msg[50];
        sprintf(soc_msg, "State of Charge: %.2f%%\n", SOC);
        pc.write(soc_msg, strlen(soc_msg));
    }
}

// Function to calculate the median value (robust to outliers)
float CalcMedian(int* arr_sv, size_t len_arr_sv) {
    // Sort the array
    std::sort(arr_sv, arr_sv + len_arr_sv);
    
    // If the length is odd, return the middle element
    if (len_arr_sv % 2 != 0) {
        return arr_sv[len_arr_sv / 2];
    }
    // If the length is even, return the average of the two middle elements
    return (arr_sv[(len_arr_sv - 1) / 2] + arr_sv[len_arr_sv / 2]) / 2.0;
}

// Low-pass filter (Exponential Smoothing)
float ApplyLowPassFilter(float current) {
    static float filteredCurrent = 0; // Keeps track of the filtered value
    
    // Apply exponential smoothing
    filteredCurrent = (ALPHA * current) + ((1.0 - ALPHA) * filteredCurrent);
    
    return filteredCurrent;
}

// Function to read and process the Hall Effect sensor data with median and low-pass filters
float HallEffectSensor() {
    float vref = 0;
    float vout = 0;
    int RefSensorValues[ITERATION];
    int OutSensorValues[ITERATION];
    float ref_voltage_V;
    float out_voltage_V;
    float voltage_diff;
    float current;

    // Read sensor values for multiple iterations for noise reduction
    for (int i = 0; i < ITERATION; i++) {
        RefSensorValues[i] = sensorPinA1.read_u16(); // Reading reference voltage
        OutSensorValues[i] = sensorPinA2.read_u16(); // Reading output voltage
        ThisThread::sleep_for(1ms);
    }

    // Apply median filter for better noise reduction
    vref = CalcMedian(RefSensorValues, ITERATION);
    vout = CalcMedian(OutSensorValues, ITERATION);

    // Convert readings to voltage (V) (since AnalogIn provides a 16-bit value)
    ref_voltage_V = (vref / pow(2, BIT_RESOLUTION)) * VOLTAGE_REFERENCE_V;
    out_voltage_V = (vout / pow(2, BIT_RESOLUTION)) * VOLTAGE_REFERENCE_V;

    // Calculate the voltage difference and current
    voltage_diff = (out_voltage_V - ref_voltage_V) / OP_AMP_GAIN;  // Adjust for amplifier gain
    current = (voltage_diff * 1000) / HALL_SENSITIVITY;  // Convert to mV and apply sensitivity

    // Print the current reading
    char current_msg[50];
    sprintf(current_msg, "Current: %.3f A\n", current);
    pc.write(current_msg, strlen(current_msg));

    return current;
}
