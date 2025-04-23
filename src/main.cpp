#include "mbed.h"
#include "adbms_main.h"
#include "common.h"

// Edge Impulse
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "edge_impulse_data.h"

// minimal working uart
// BufferedSerial uart(D6, D7);  // TX, RX
// char rx_buffer[64];
// int rx_index = 0;  // Must be global so it doesn't reset every loop

// int main()
// {
//     uart.set_baud(9600);
//     printf("Waiting for UART messages from nRF52...\n");

//     while (true) {
//         if (uart.readable()) {
//             char c;
//             if (uart.read(&c, 1)) {
//                 if (c == '\n' || rx_index >= sizeof(rx_buffer) - 1) {
//                     rx_buffer[rx_index] = '\0';  // Null-terminate
//                     printf("Received: %s\n", rx_buffer);
//                     rx_index = 0;  // Reset for next line
//                 } else {
//                     rx_buffer[rx_index++] = c;
//                 }
//             }
//         }

//         ThisThread::sleep_for(10ms);  // Give the MCU a breather
//     }
// }

// Edge Impulse ML sampling config
#define SAMPLE_COUNT         500     // EI_CLASSIFIER_RAW_SAMPLE_COUNT
#define NUM_AXES             3       // EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME
#define SAMPLE_FREQUENCY_HZ  10.0f   // EI_CLASSIFIER_FREQUENCY (Hz)
#define TOTAL_SAMPLES        (SAMPLE_COUNT * NUM_AXES)
#define SLEEP_TIME_MS        (1000 / SAMPLE_FREQUENCY_HZ)

// UART pins (connected to TX from nRF52DK)
BufferedSerial uart(A0, D7); // TX, RX
float raw_data_buffer[TOTAL_SAMPLES] = {};  // Global inference buffer
char rx_line[64] = {};
int sample_idx = 0;
int rx_index = 0;

// Classifier callback
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
    memcpy(out_ptr, raw_data_buffer + offset, length * sizeof(float));
    return 0;
}

// Replace with actual values from stats
float mean[3] = { 3.66746668, -0.04237708, 15.46597781 };
float stddev[3] = { 0.38206392, 3.74553363, 11.89024301 };

float last_voltage = 0.0f;
float last_current = 0.0f;
float last_temp = 0.0f;

// coulomb counting
float cumulative_ah = 0.0f;
const float delta_t = 1.0f / SAMPLE_FREQUENCY_HZ;  // 0.1s for 10Hz
const float nominal_capacity_ah = 5.0f;
float soc_coulomb = 100.00f;

// Read and parse incoming UART sensor data line-by-line
void uart_fill_buffer_from_serial(BufferedSerial &uart) {
    sample_idx = 0;  // Reset buffer index

    while (sample_idx < SAMPLE_COUNT) {
        if (uart.readable()) {
            char c;
            if (uart.read(&c, 1)) {
                if (c == '\n') {
                    rx_line[rx_index] = '\0';
        
                    float voltage, current, temp;

                    if (sscanf(rx_line, "%f,%f,%f", &voltage, &current, &temp) == 3) {
                        int offset = sample_idx * NUM_AXES;
                        raw_data_buffer[offset + 0] = (voltage - mean[0]) / (stddev[0] + 1e-6);
                        raw_data_buffer[offset + 1] = (current - mean[1]) / (stddev[1] + 1e-6);
                        raw_data_buffer[offset + 2] = (temp    - mean[2]) / (stddev[2] + 1e-6);
                    
                        // 🆕 Update latest values
                        last_voltage = voltage;
                        last_current = current;
                        last_temp = temp;
                    
                        // ✅ Coulomb counting at 10Hz (every sample)
                        float delta_ah = current * delta_t / 3600.0f;
                        cumulative_ah += delta_ah;

                        // SoC from Coulomb counting (starting at 100%)
                        soc_coulomb = 100.0f * (1.0f + cumulative_ah / nominal_capacity_ah);
                    
                        sample_idx++;
                    }
                    
                    rx_index = 0;
                }
                else if (rx_index < (int)(sizeof(rx_line) - 1)) {
                    rx_line[rx_index++] = c;
                }
                else {
                    // Prevent buffer overflow
                    rx_index = 0;
                }
            }
        }
    }
}

// Inference results buffer
ei_impulse_result_t result;

// Simulation state
float sim_voltage = 4.2f;
float sim_current = -2.0f;
int sample_phase = 0;
int test_stage = 0;

void simulate_sensors() {
    const float temp = 25.0f;
    const float voltage_step = 0.1f;
    const float current_step = 0.1f;
    const float v_max = 4.2f;
    const float v_min = 2.5f;
    const float i_max =  2.0f;
    const float i_min = -2.0f;

    sample_idx = 0;
    while (sample_idx < SAMPLE_COUNT) {
        int offset = sample_idx * NUM_AXES;
        raw_data_buffer[offset + 0] = (sim_voltage - mean[0]) / (stddev[0] + 1e-6);
        raw_data_buffer[offset + 1] = (sim_current - mean[1]) / (stddev[1] + 1e-6);
        raw_data_buffer[offset + 2] = (temp - mean[2]) / (stddev[2] + 1e-6);

        if (sample_idx == 0) {
            last_voltage = sim_voltage;
            last_current = sim_current;
            last_temp = temp;
        }

        sample_idx++;
    }

    sample_phase++;
    if (test_stage == 0) {
        sim_voltage -= voltage_step;
        if (sim_voltage <= v_min) {
            sim_voltage = v_min;
            test_stage++;
        }
    } else if (test_stage == 1) {
        sim_current += current_step;
        if (sim_current >= -0.1f) {
            sim_current = 2.0f;
            test_stage++;
        }
    } else if (test_stage == 2) {
        sim_voltage += voltage_step;
        if (sim_voltage >= v_max) {
            sim_voltage = v_max;
            test_stage++;
        }
    } else if (test_stage == 3) {
        sim_current -= current_step;
        if (sim_current <= 0.1f) {
            sim_current = 0.1f;
            test_stage = 0;
            sim_voltage = v_max;
        }
    }
}

void run_inference() {
    printf("voltage,current,battery_temp,ML_SoC,soc_coulomb\n");

    while (true) {
        // simulate_sensors();
        uart_fill_buffer_from_serial(uart);

        signal_t signal;
        signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
        signal.get_data = &raw_feature_get_data;

        EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
        if (res != EI_IMPULSE_OK) continue;

        float predicted_soc = result.classification[0].value;

        // Output
        printf("%.3f,%.3f,%.3f,%.5f,%.6f\n",
            last_voltage, last_current, last_temp,
            predicted_soc, soc_coulomb);     
    }
}

int main() {
    uart.set_baud(9600);

    // minimal snippet:
    // uart.set_baud(9600);
    // printf("👂 UART Monitor Started (9600 baud, D6 = RX)\n");
    // while (true) {
    //     if (uart.readable()) {
    //         char c;
    //         if (uart.read(&c, 1)) {
    //             if (c == '\n') {
    //                 rx_line[rx_index] = '\0';
    //                 printf("🔹 Received: %s\n", rx_line);
    //                 rx_index = 0;
    //             } else if (rx_index < sizeof(rx_line) - 1) {
    //                 rx_line[rx_index++] = c;
    //             } else {
    //                 // Overflow protection
    //                 rx_index = 0;
    //             }
    //         }
    //     }
    // }

    // To log results, run in PlatformIO shell:
    // pio device monitor --port COM14 --baud 9600 --filter default --filter time --filter log2file

    run_inference();
}
