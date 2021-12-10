/***********************************************************************************
* @file statemachine.c
 * @brief:Build a state machine that can handle and process various events and states
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
#include "bme280.h"
#include "statemachine.h"
//***********************************************************************************
//                                  Macros
//***********************************************************************************
//***********************************************************************************
//                              Structures
//***********************************************************************************


event_e event;
state_e state = STATE_IDLE;
sensor_val_t sensor_val = {0};

//***********************************************************************************
//                                  Function definition
//***********************************************************************************
void set_timer_event()
{
	event |= TIMER_EVENT;
}

event_e get_event()
{
	event_e temp_event = 0;
	if(event & TIMER_EVENT)
	{
		temp_event = TIMER_EVENT;
		event &= ~(TIMER_EVENT);
	}

	return temp_event;
}
/*-----------------------------------------------------------------------------------------------------------------------------*/
/*
 @brief: State machine to manage weather monitoring station
 @param: None
 @return:None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
void weather_monitor_statemachine()
{
	//Get event
	event_e event = get_event();

	switch(state)
	{

		case STATE_IDLE:
		{
			if(event == TIMER_EVENT)
			{
				state = STATE_READ_SENSORS;
			}
		}
		break;

		case STATE_READ_SENSORS:
		{
			read_sensors(&sensor_val);

			state = STATE_TRANSMIT_VAL;
		}
		break;

		case STATE_TRANSMIT_VAL:
		{
			transmit_sensors_val(&sensor_val);
			state = STATE_IDLE;
		}
		break;
	}

}


