/*******************************************************************************
Copyright (c) 2020 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensor.
------------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "adbms_main.h"
#include "common.h"

/*
https://os.mbed.com/platforms/SDP_K1/
Mbed SDP-K1 board MOSI, MOSI, SCK definition.
Arduino Header Pinout
MOSI = D11`
MISO = D12
SCK  = D13
CSB =  D10
*/
int main(void);
void spi_init(void);

DigitalOut chip_select(D10);                /* SPI chip select */
DigitalOut mosi(D11);                       /* MOSI */
DigitalIn miso(D12);                        /* MISO */
DigitalOut sclk(D13);                       /* SCK */
SPI spi(D11, D12, D13);                     /* SPI MOSI, MISO, SCK */
Timer timer;


// int main()
// {
//     wait_us(1000000);
//     printf("Initialization Check.\n");
//     wait_us(1000000);

//     spi_init();
//     adbms_main();

//     return 0;
// }

int main() {
    ThisThread::sleep_for(1s);  // Let USB serial settle
    printf("=== SPI Basic Line Check ===\n");

    // Setup SPI: 8-bit, Mode 0, 2 MHz
    spi.format(8, 0);           
    spi.frequency(2000000);     

    chip_select = 1;
    ThisThread::sleep_for(5ms); // Ensure idle CS before start

    chip_select = 0;
    wait_us(5);                 // CS settle before SPI
    uint8_t received = spi.write(0xAA);  // Send dummy byte
    wait_us(5);
    chip_select = 1;

    printf("Sent: 0x%02X, Received: 0x%02X\n", 0xAA, received);

    while (true) {
        ThisThread::sleep_for(1s);
    }
}

void spi_init()
{   
    chip_select = 1;
    spi.format(8,0);            /* 8bit data, CPOL-CPHA= 0*/
    spi.frequency(2000000);     /* SPI clock 2Mhz */
}