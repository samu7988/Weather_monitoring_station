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

/*--------------------------------------------------------------------------------------------------------------------------*/
/*
  @brief: Reads a single character from SPI0_D register
 @param: Pointer into which data is read.
 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/

void SPI_read_byte(uint8_t* data){

	while((SPI0->S & SPI_S_SPRF_MASK) != (SPI_S_SPRF_MASK)); //Wait until SPI read buffer flag is full

	*data = SPI0->D; //Copy the data register into a variable
}

/*--------------------------------------------------------------------------------------------------------------------------*/
/*
  @brief: Send a single character into SPI0_D register
 @param: Value that is to be sent
 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
void SPI_write_byte(uint8_t data){

	while((SPI0_S & SPI_S_SPTEF_MASK) !=(SPI_S_SPTEF_MASK)); //Wait until SPI transit buffer flag is set
	SPI0->D=byte;

}

/*--------------------------------------------------------------------------------------------------------------------------*/
/*
  @brief: Send multibyte value via SPI
 @param: *data: Pointer to value that is to be sent
 	 	 length: length of data that is to be sent.
 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
void SPI_write_multibyte(uint8_t* data, size_t length)
{
	for(int i = 0; i < length ;i++)
	{
		SPI_write_byte(data[i]);
	}
}

/*--------------------------------------------------------------------------------------------------------------------------*/
/*
  @brief: Read multibyte value via SPI
 @param: *data: Buffer into which value is to be read
 	 	 length: length of buffer
 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
void SPI_read_multibyte(uint8_t* data, size_t length)
{
	for(int i = 0; i < length ; i++)
	{
		SPI_read_byte(&data[i]);
	}
}