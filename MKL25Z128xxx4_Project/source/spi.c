/*
 * spi.c
 *
 *  Created on: Dec 4, 2021
 *      Author: mules
 */

/***********************************************************************************
* @file cbfifo.c
 * @brief:a fixed-size FIFO implemented via a circular buffer
 *        1)operation on fifo using circular buffer
 *        2)creating circular buffer
 *        3)enqueing of circular buffer
 *        4)Dequeing of circular buffer
 *        5)Returns the length of the circular buffer
 *        6)returns the capacity of the circular buffer
 * @author Sayali Mule
 * @date 09/05/2021
 * @Reference:
 *
 *****************************************************************************/
//***********************************************************************************
//                              Include files
//***********************************************************************************
#include "MKL25Z4.h"

//***********************************************************************************
//                                  Macros
//***********************************************************************************
//***********************************************************************************
//                              Structures
//***********************************************************************************



//***********************************************************************************
//                                  Function definition
//***********************************************************************************


/*-----------------------------------------------------------------------------------------------------------------------------*/
/*
 @brief: Initialise SPI peripheral
 @param: None
 @return:None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/


void spi_init()
{

	SIM->SCGC5 |=SIM_SCGC5_PORTD_MASK;// Enable clock to PORTD which has multiplexed SPI pins(Bit 12)
	SIM->SCGC4 |=SIM_SCGC4_SPI0_MASK; //Enable clock to SPI0 module(Bit 22)


	PORTD->PCR[1] |= PORT_PCR_MUX(2); //PTD1->SCK
	PORTD->PCR[3] |= PORT_PCR_MUX(2);  //PTD3->MISO
	PORTD->PCR[2] |= PORT_PCR_MUX(2); //PTD2->MOSI
	PORTD->PCR[0] |= PORT_PCR_MUX(1);//PTD0-> CS



	SPI0->C1 |= SPI_C1_MSTR_MASK; //Enable master mode
	SPI0->C2 &= ~(SPI_C2_MODFEN_MASK); //Make MODFEN bit to zero

	SPI0->BR = (SPI_BR_SPPR(0x00) | SPI_BR_SPR(0x01)); //SPR(0x01)-> Baud rate divisor = 4, SPPR(0x00)->Prescalar divisor is 1
	SPI0->C1 |= SPI_C1_SPE_MASK; //Enable spi
}
