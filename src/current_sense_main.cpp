#include "mbed.h"

// NRF52-DK uses specific pins. Replace with the correct pins for your MCU.
AnalogIn sensorPinA0(A0); // Define the analog pin (Replace with actual pin names if needed)
AnalogIn sensorPinA1(A1);
AnalogIn sensorPinA2(A2);
BufferedSerial pc(USBTX, USBRX, 9600); // Use BufferedSerial for serial communication

const float SHUNT_CURRENT_A = 10.00;
const float SHUNT_VOLTAGE_MV = 100.0;
const float CORRECTION_FACTOR = 0.00;
const float HALL_SENSITIVITY = 80.0;
const float OP_AMP_GAIN = 4.9819;

const int ITERATION = 20; 
const float VOLTAGE_REFERENCE_MV = 3000.00; // 3V reference
const int BIT_RESOLUTION = 10 ;
const int MAX_SENSOR_VALUE = 100; // Define the maximum sensor value

// SOC Global Variables
const float BATTERY_CAP_AH = 5.0;
float SOC = 100.0; // Initial SOC
unsigned long prevMillis = 0;
const long interval = 1000; // 1000ms to 1sec
float totalCoulombs = 0.0;

void printCurrent();
float HallEffectSensor();
float CalcTruncatedMean(int* arr_sv, size_t len_arr_sv);

int current_sense_main() {
    // Startup message
    pc.write("10A Current Shunt Sensor\n", 26);
    ThisThread::sleep_for(500ms);

    while (true) {
        unsigned long currentMillis = Kernel::get_ms_count(); // Use Mbed's time tracking

        if (currentMillis - prevMillis >= interval) {
            prevMillis = currentMillis;

            float current_a = HallEffectSensor();

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

            ThisThread::sleep_for(1000ms);
        }
    }
}

float CalcTruncatedMean(int* arr_sv, size_t len_arr_sv) {
    float final_avg = 0;
    int min_so_far = 16384;
    int max_so_far = 0;

    for (size_t idx = 0; idx < len_arr_sv; idx++) {
        int current_value = arr_sv[idx];
        if (current_value < min_so_far) {
            min_so_far = current_value;
        }
        if (current_value > max_so_far) {
            max_so_far = current_value;
        }
        final_avg += current_value;
    }

    final_avg -= min_so_far;
    final_avg -= max_so_far;
    final_avg = final_avg / (len_arr_sv - 2); // Remove the highest and lowest from the array
    return final_avg;
}

float HallEffectSensor() {
    float vref = 0;
    float vout = 0;
    int RefSensorValues[ITERATION];
    int OutSensorValues[ITERATION];
    float ref_voltage_mV;
    float out_voltage_mV;
    float voltage_diff;
    float current;

    for (int i = 0; i < ITERATION; i++) {
        RefSensorValues[i] = sensorPinA1.read_u16(); // Reading reference voltage
        OutSensorValues[i] = sensorPinA2.read_u16(); // Reading output voltage
        ThisThread::sleep_for(1ms);
    }

    vref = CalcTruncatedMean(RefSensorValues, ITERATION);
    vout = CalcTruncatedMean(OutSensorValues, ITERATION);

    ref_voltage_mV = (vref) * (VOLTAGE_REFERENCE_MV / (pow(2, BIT_RESOLUTION) - 1));
    out_voltage_mV = (vout) * (VOLTAGE_REFERENCE_MV / (pow(2, BIT_RESOLUTION) - 1));

    voltage_diff = out_voltage_mV - ref_voltage_mV;
    current = voltage_diff / HALL_SENSITIVITY;

    char current_msg[50];
    sprintf(current_msg, "Current: %.3f A\n", current);
    pc.write(current_msg, strlen(current_msg));

    return current;
}
