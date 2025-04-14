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
float mean[3] = { 3.66729629, -0.77352798, 8.94461763 };
float stddev[3]  = { 0.30899151, 1.81825831, 13.95914506 };

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
                        raw_data_buffer[offset + 2] = (temp    - mean[2]) / (stddev[2]  + 1e-6);
        
                        if (sample_idx % 50 == 0) {
                            printf("Sample %d: V=%.2f, I=%.2f, T=%.2f\n", sample_idx, voltage, current, temp);
                        }
        
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

void run_inference() {
    printf("\n=== Edge Impulse: Real-Time Inference via UART ===\n");
    printf("Expecting %d samples with %d axes (total %d values) at %.2f Hz\n\n",
           SAMPLE_COUNT, NUM_AXES, TOTAL_SAMPLES, SAMPLE_FREQUENCY_HZ);

    while (true) {
        printf("📡 Collecting sensor window via UART...\n");
        uart_fill_buffer_from_serial(uart);

        // 🟡 DEBUG: Print first sample in raw_data_buffer
        printf("   Normalized: %.3f, %.3f, %.3f\n",
            raw_data_buffer[0], raw_data_buffer[1], raw_data_buffer[2]);        

        signal_t signal;
        signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
        signal.get_data = &raw_feature_get_data;

        EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
        if (res != EI_IMPULSE_OK) {
            printf("⚠️  run_classifier failed (%d)\n", res);
            continue;
        }

        printf("✅ Inference results:\n");
        printf("   Regression output: %.5f\n", result.classification[0].value);

    #if EI_CLASSIFIER_HAS_ANOMALY == 1
        printf("   anomaly: %.3f\n", result.anomaly);
    #endif

        printf("\n⏳ Waiting before next inference...\n\n");
        ThisThread::sleep_for(1000ms);  // Optional pause between inferences
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

    // real function:
    printf("👋 Starting UART-to-ML Inference on nRF52840...\n");
    run_inference();
}
