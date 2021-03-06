/***********************************************************************************
* @file uart.h
 * @brief: Contains code to initialise UART, and uart Tx and Rx
 * @author Sayali Mule
 * @date 11/03/2021
 * @Reference:
 *
 *****************************************************************************/
#ifndef UART_H_
#define UART_H_
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
void uart0_init();
void uart1_init();
void uart1_puts(uint8_t* msg);
void my_itoa(size_t num, uint8_t* input);

#endif /* UART_H_ */
