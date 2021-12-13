///*
// * Copyright 2016-2021 NXP
// * All rights reserved.
// *
// * Redistribution and use in source and binary forms, with or without modification,
// * are permitted provided that the following conditions are met:
// *
// * o Redistributions of source code must retain the above copyright notice, this list
// *   of conditions and the following disclaimer.
// *
// * o Redistributions in binary form must reproduce the above copyright notice, this
// *   list of conditions and the following disclaimer in the documentation and/or
// *   other materials provided with the distribution.
// *
// * o Neither the name of NXP Semiconductor, Inc. nor the names of its
// *   contributors may be used to endorse or promote products derived from this
// *   software without specific prior written permission.
// *
// * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// */
//
///**
// * @file    Weather_monitor_station.c
// * @brief   Application entry point.
// */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "gpio.h"
#include "bme280.h"
#include "spi.h"
#include "uart.h"
#include "sysclock.h"
#include "cbfifo.h"
#include "systick.h"
#include "statemachine.h"

#define ENABLE_LOGGING (1)


/*
 * @brief   Application entry point.
 */
int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    /***************************************************
     * 	       PERIPHERAL INITIALISATION
     **************************************************/
    int status = 0;
	uart0_init(); //Initialise UART0(Needed for enable console logging
	gpio_init();  // PD0 acts as SPI chip select(initialise PD0)
    spi_init();   //Initialise SPI with CPHA=CPOL=0

    uart1_init(); //Bluetooth module uses UART1 for sending data
    systick_init(); //Timer initialisation that fires every 3 seconds

    //Create the Tx handle that points to tx buffer(statically allocated)
    status |= create_tx_cb_handle();


    /***********************************************************************
     * 	 Test whether Environmental sensor is connected by reading chip ID
     ***********************************************************************/
    uint8_t chip_id = bme280_init();

    if(chip_id != CHIP_REV)
    {
    	printf("The sensor did not respond with correct chip id val,Please check the connection\n\r");
    }
    else
    {
    	printf("BME280 sensor initialization is successfull\n\r");
    }


    /* Enter an infinite loop, just incrementing a counter. */
    while(1)
    {
    	/**********************************
    	 * Run weather monitor state machine
    	 * 1) Wait for timer event to fire
    	 * 2) When timer event occurs, read sensor data(temp, humidity and pressure)
    	 * 3) Send the values of sensor to bluetooth using UART1
    	 ***********************************/

    	weather_monitor_statemachine();
//    	uart1_puts("Vikrant\n");

    }
    return 0 ;

}




