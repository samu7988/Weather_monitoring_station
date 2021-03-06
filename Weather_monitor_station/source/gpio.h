/***********************************************************************************
* @file gpio.h
 * @brief:Initialise GPIO, GPIO turn on, GPIO turn off
 * @author Sayali Mule
 * @date 12/04/2021
 * @Reference:
 *****************************************************************************/
#ifndef GPIO_H_
#define GPIO_H_
//***********************************************************************************
//                              Include files
//***********************************************************************************
#include <stdint.h>
#include "MKL25Z4.h"
//***********************************************************************************
//                                  Macros
//***********************************************************************************
#define SPI_CS_PORT	(GPIOD)
#define SPI_CS_PIN  (0)

#define GREEN_LED_PORT  (GPIOB)
#define GREEN_LED_PIN   (19)

//***********************************************************************************
//                                  Function Prototype
//***********************************************************************************
void gpio_init();
void gpio_on(GPIO_Type* port, uint8_t pin);
void gpio_off(GPIO_Type* port, uint8_t pin);
void gpio_toggle(GPIO_Type* port, uint8_t pin);
#endif /* GPIO_H_ */
