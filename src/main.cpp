/*******************************************************************************
Copyright (c) 2020 - Analog Devices Inc.
All Rights Reserved. Proprietary & Confidential.
------------------------------------------------------------------------------*/

#include "adbms_main.h"
#include "common.h"
#include "adBms6830GenericType.h"

// Use SPIM0 default pins (guaranteed to work with Mbed)
SPI spi(P1_13, P1_14, P1_15);  // MOSI, MISO, SCK
DigitalOut chip_select(P1_12); // CS pin
Timer timer;

void spi_init(void);

int main()
{
    wait_us(1000000);
    printf("Initialization Check.\n");
    wait_us(1000000);

    spi_init();
    adbms_main();

    return 0;
}

// int main() {
//     ThisThread::sleep_for(1s);  // Let USB serial settle
//     printf("=== SPI Basic Line Check ===\n");

//     // Setup SPI: 8-bit, Mode 0, 2 MHz
//     spi.format(8, 0);           
//     spi.frequency(2000000);     

//     chip_select = 1;
//     ThisThread::sleep_for(5ms); // Ensure idle CS before start

//     chip_select = 0;
//     wait_us(5);                 // CS settle before SPI
//     uint8_t received = spi.write(0xAA);  // Send dummy byte
//     wait_us(5);
//     chip_select = 1;

//     printf("Sent: 0x%02X, Received: 0x%02X\n", 0xAA, received);

//     while (true) {
//         ThisThread::sleep_for(1s);
//     }
// }

void spi_init()
{   
    chip_select = 1;                // Idle state for CS
    spi.format(8, 0);               // 8-bit data, SPI mode 0 (CPOL=0, CPHA=0)
    spi.frequency(2000000);         // SPI clock 2 MHz
}
