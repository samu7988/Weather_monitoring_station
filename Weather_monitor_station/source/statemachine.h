/***********************************************************************************
* @file statemachine.h
 * @brief:Contains weather monitor statemachine and routines for scheduling events
 * @author Sayali Mule
 * @date 12/04/2021
 * @Reference:
 *****************************************************************************/
#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_
//***********************************************************************************
//                              Include files
//***********************************************************************************

//***********************************************************************************
//                                  Macros
//***********************************************************************************
typedef enum
{
	TIMER_EVENT = 1,
}event_e;

typedef enum
{
	STATE_IDLE = 1,
	STATE_READ_SENSORS = 2,
	STATE_TRANSMIT_VAL =  4
}state_e;

//***********************************************************************************
//                                  Function Prototype
//***********************************************************************************
void set_timer_event();
event_e get_event();
void weather_monitor_statemachine();
#endif /* STATEMACHINE_H_ */
