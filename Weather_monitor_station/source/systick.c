/***********************************************************************************
* @file   systick.c
 * @brief: Sets systick peripheral and its interrupt service routine
 * @author Sayali Mule
 * @date 10/06/2021
 * @Reference:https://github.com/alexander-g-dean/ESF/blob/master/NXP/Misc/Touch%20Sense/TSI/src/main.c
 *
 *****************************************************************************/

//***********************************************************************************
//                              Include files
//***********************************************************************************
#include "MKL25Z4.h"
#include "systick.h"
#include "gpio.h"
#include "fsl_debug_console.h"

//***********************************************************************************
//                                  Macros
//***********************************************************************************



//***********************************************************************************
//                                  Enum
//***********************************************************************************


//***********************************************************************************
//                                  Structure
//***********************************************************************************

/*-----------------------------------------------------------------------------------------------------------------------------*/
/*
 @brief: Systick Interrupt handler
@param: None
 @return:None
 @Reference:
-------------------------------------------------*/
void SysTick_Handler(void){

	gpio_toggle(GREEN_LED_PORT, GREEN_LED_PIN);
	int x =0;
//	printf("Systick handler\n\r");
	x =6;
}

/*-----------------------------------------------------------------------------------------------------------------------------*/
/*
 @brief: Initialise the systick peripheral
@param: delay_ms: Can only count uptil 349msec
 @return:None
 @Reference:
 -------------------------------------------------------------------------------*/

void systick_init(uint32_t delay_ms)
{


	SysTick->LOAD = (48000000L)/4;
	NVIC_SetPriority(SysTick_IRQn, 3);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}




