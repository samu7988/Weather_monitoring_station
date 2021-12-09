/***********************************************************************************
* @file spi.h
 * @brief:Initialise SPI peripheral, SPI read and write API
 * @author Sayali Mule
 * @date 12/04/2021
 * @Reference:
 *****************************************************************************/
#ifndef SPI_H_
#define SPI_H_
//***********************************************************************************
//                              Include files
//***********************************************************************************
#include <stdint.h>
//***********************************************************************************
//                                  Macros
//***********************************************************************************



//***********************************************************************************
//                                  Function Prototype
//***********************************************************************************
void spi_init();
void SPI_read_byte(uint8_t* data);
uint8_t SPI_write_byte(uint8_t data);
void SPI_write_multibyte(uint8_t* data, size_t length);
void SPI_read_multibyte(uint8_t* data, size_t length);
void SPI_read_register(uint8_t reg_addr,uint8_t* read_data);
void SPI_write_register(uint8_t reg_addr, uint8_t data);


#endif /* SPI_H_ */
