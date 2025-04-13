/*******************************************************************************
Copyright (c) 2020 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensor.
------------------------------------------------------------------------------*/

#include "mbed.h"    // <-- This brings in DigitalOut, DigitalIn, SPI, Timer, wait_us(), etc.
#include "adbms_main.h"
#include "common.h"

/*
https://os.mbed.com/platforms/SDP_K1/
Mbed SDP-K1 board MOSI, MISO, SCK definition.
Arduino Header Pinout
MOSI = D11
MISO = D12
SCK  = D13
CSB =  D10
*/

// Set up UART: RX from nRF52 TX (use correct pins for your board)
UnbufferedSerial uart(P0_8, P0_6, 9600); // RX, TX, baud

char rx_buffer[64];
int index = 0;

int main(void);
void spi_init(void);

DigitalOut chip_select(D10);    // SPI chip select
DigitalOut mosi(D11);           // MOSI
DigitalIn  miso(D12);           // MISO
DigitalOut sclk(D13);           // SCK
SPI spi(D11, D12, D13);         // SPI MOSI, MISO, SCK
Timer timer;

int main()
{
    printf("Waiting for UART messages from nRF52...\n");

    while (true) {
        if (uart.readable()) {
            char c;
            if (uart.read(&c, 1)) {
                if (c == '\n' || index >= sizeof(rx_buffer) - 1) {
                    rx_buffer[index] = '\0';  // Null-terminate
                    printf("Received: %s\n", rx_buffer);
                    index = 0;  // Reset for next message
                } else {
                    rx_buffer[index++] = c;
                }
            }
        }

        ThisThread::sleep_for(10ms);  // Don't hammer the CPU
    }

    // wait_us(1000000);
    // printf("Initialization Check.\n");
    // wait_us(1000000);

    // spi_init();
    // adbms_main();

    // return 0;
}

void spi_init()
{   
    chip_select = 1;
    spi.format(8, 0);            // 8-bit data, CPOL=0, CPHA=0
    spi.frequency(2000000);        // SPI clock 2MHz
}
