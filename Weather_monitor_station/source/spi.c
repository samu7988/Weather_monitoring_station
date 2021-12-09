
/***********************************************************************************
* @file spi.c
 * @brief:Initialise SPI peripheral, SPI read and write API
 * @author Sayali Mule
 * @date 12/04/2021
 * @Reference:
 *
 *****************************************************************************/
//***********************************************************************************
//                              Include files
//***********************************************************************************
#include "MKL25Z4.h"
#include "gpio.h"

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
	 /*
	  * PD0 -> CS
	 * PD1 -> SCK
	 * PD3 -> MISO
	 * PD2 -> MOSI
	 */

	SIM->SCGC5 |=SIM_SCGC5_PORTD_MASK;// Enable clock to PORTD which has multiplexed SPI pins(Bit 12)
	SIM->SCGC4 |=SIM_SCGC4_SPI0_MASK; //Enable clock to SPI0 module(Bit 22)


	PORTD->PCR[1] |= PORT_PCR_MUX(2); //PTD1->SCK
	PORTD->PCR[3] |= PORT_PCR_MUX(2);  //PTD3->MISO
	PORTD->PCR[2] |= PORT_PCR_MUX(2); //PTD2->MOSI



	SPI0->C1 |= SPI_C1_MSTR_MASK; //Enable master mode
	SPI0->C1 &= ~(SPI_C1_CPHA_MASK); //Make CPOL 0
	SPI0->C1 &= ~(SPI_C1_CPOL_MASK); //Make CPHA 0
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
uint8_t SPI_write_byte(uint8_t data)
{

	while((SPI0->S & SPI_S_SPTEF_MASK) !=(SPI_S_SPTEF_MASK)); //Wait until SPI transit buffer is empty
	SPI0->D= data;
	while((SPI0->S & SPI_S_SPRF_MASK) != (SPI_S_SPRF_MASK)); //Wait until Tx is complete
	uint8_t dummy = SPI0->D;
	return dummy;


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
		SPI_read_register(&data[i]);
	}
}

/*------------------------------------------------------------------------*/
/*
  @brief: Read a specific register using SPI
 @param: reg_addr: Register addr that is to be read
 	 	 read_data: Pointer variable in which data is to be stored
 @return: None
 */
/*-----------------------------------------------------------------------*/
void SPI_read_register(uint8_t reg_addr,uint8_t* read_data)
{
	uint8_t dummy_data = 0;
	gpio_off(SPI_CS_PORT, SPI_CS_PIN); //Turn CS low

	SPI_write_byte(reg_addr); //Write reg addr

//	SPI_read_byte(&dummy_data); //read dummy data

	*read_data =  SPI_write_byte(0xFF);

//	SPI_read_byte(read_data); //read data from specified reg. addr

	gpio_on(SPI_CS_PORT, SPI_CS_PIN); //Turn CS high

}

/*------------------------------------------------------------------------*/
/*
  @brief: Write to a specific register using SPI
 @param: reg_addr: Register addr that is to be read
 	 	 read_data: Pointer variable in which data is to be stored
 @return: None
 */
/*-----------------------------------------------------------------------*/
void SPI_write_register(uint8_t reg_addr, uint8_t data)
{
	gpio_off(SPI_CS_PORT, SPI_CS_PIN); //Turn CS low

	SPI_write_byte(reg_addr); //Write reg addr

	SPI_write_byte(data); //write data to specified reg. addr

	gpio_on(SPI_CS_PORT, SPI_CS_PIN); //Turn CS high
}
