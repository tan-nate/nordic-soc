#include "common.h"
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

const int ITERATION = 50;      // Increased number of samples for better filtering
const float VOLTAGE_REFERENCE_V = 3.00; // 3V reference (verify for your board)
const int BIT_RESOLUTION = 16;  // 16-bit ADC resolution (verify for your board)

// Low-Pass Filter Constant
const float ALPHA = 0.1;  // Alpha value for exponential smoothing

// SOC Global Variables
const float BATTERY_CAP_AH = 5.0;
float SOC = 100.0;           // Initial SOC
unsigned long prevMillis = 0;
const long interval = 1000;  // 1000ms = 1 sec
float totalCoulombs = 0.0;

// Timer object to handle periodic calls
Timer currentSenseTimer;

// Function prototypes
void printCurrent();
float HallEffectSensor();
float CalcMedian(int* arr_sv, size_t len_arr_sv);
float ApplyLowPassFilter(float current);

// Main function for current sensing
void current_sense_main()
{
    // Start the timer if it hasn't been started yet
    if (currentSenseTimer.elapsed_time().count() == 0) {
        currentSenseTimer.start();
        // Print a one-time startup message
        printf("10A Current Shunt Sensor\n");
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
        float charge = current_a * (interval / 1000.0f);
        totalCoulombs += charge;

        float socChange = (charge / 3600.0f) / BATTERY_CAP_AH * 100.0f;

        // Update the SOC
        SOC += socChange;
        if (SOC > 100.0f) {
            SOC = 100.0f;
        } else if (SOC < 0.0f) {
            SOC = 0.0f;
        }

        // Print the SOC
        printf("State of Charge: %.2f%%\n", SOC);
    }
}

// Function to calculate the median value (robust to outliers)
float CalcMedian(int* arr_sv, size_t len_arr_sv)
{
    std::sort(arr_sv, arr_sv + len_arr_sv);

    // If the length is odd, return the middle element
    if (len_arr_sv % 2 != 0) {
        return static_cast<float>(arr_sv[len_arr_sv / 2]);
    }
    // If the length is even, return the average of the two middle elements
    return static_cast<float>((arr_sv[(len_arr_sv - 1) / 2] + arr_sv[len_arr_sv / 2]) / 2.0);
}

// Low-pass filter (Exponential Smoothing)
float ApplyLowPassFilter(float current)
{
    static float filteredCurrent = 0.0f; // Keeps track of the filtered value
    
    // Apply exponential smoothing
    filteredCurrent = (ALPHA * current) + ((1.0f - ALPHA) * filteredCurrent);
    
    return filteredCurrent;
}

// Function to read and process the Hall Effect sensor data with median and low-pass filters
float HallEffectSensor()
{
    float vref = 0.0f;
    float vout = 0.0f;
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

    // Apply median filters
    vref = CalcMedian(RefSensorValues, ITERATION);
    vout = CalcMedian(OutSensorValues, ITERATION);

    // Convert ADC readings to voltage
    ref_voltage_V = (vref / (1 << BIT_RESOLUTION)) * VOLTAGE_REFERENCE_V;
    out_voltage_V = (vout / (1 << BIT_RESOLUTION)) * VOLTAGE_REFERENCE_V;

    // Calculate voltage difference and current
    voltage_diff = (out_voltage_V - ref_voltage_V) / OP_AMP_GAIN; 
    current = (voltage_diff * 1000.0f) / HALL_SENSITIVITY; 

    // Print the current reading
    printf("Current: %.3f A\n", current);

    return current;
}
