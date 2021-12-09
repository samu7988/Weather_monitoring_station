/***********************************************************************************
* @file uart.c
 * @brief: Contains code to initialise UART, and uart Tx and Rx

 * @author Sayali Mule
 * @date 11/03/2021
 * @Reference:
 *
 *****************************************************************************/
//***********************************************************************************
//                              Include files
//***********************************************************************************
#include "MKL25Z4.h"
#include "sysclock.h"
#include "uart.h"
#include "cbfifo.h"
#include <string.h>
//***********************************************************************************
//                                  Macros
//***********************************************************************************
#define UART_OVERSAMPLE_RATE (16)

#define BAUD_RATE (38400)
#define STOP_BITS	(1) //0->1 stop bit, 1->2 stop bits
#define PARITY		(0) //0->No parity
#define DATA_BITS   (0) //0->8 bits, 1->9bits

//***********************************************************************************
//                                  Function definition
//***********************************************************************************
void UART0_IRQHandler(void)
{

	__disable_irq();     //critical section
	cb_error_status_e cb_status = CB_INSTANCE_SUCCESS;

	//if any character is received in data register, then Rx interrupt will be triggered
	if(UART0->S1 & UART0_S1_RDRF_MASK) //when data register is full
	{
		uint8_t rcvd_val = 0;
		rcvd_val = UART0->D;
		cb_status = cbfifo_enqueue(RX_BUFFER,&rcvd_val); //Enqueue the received data into Rx buffer(accessed via handler)
	}
	else if((UART0->S1 & UART0_S1_TDRE_MASK)) //when data register is empty
	{
		uint8_t tx_val = 0;
		//if previous trasnmission is complete, then dequeue a character from Tx buffer and start with another transmission
		if(UART0->S1 & UART0_S1_TC_MASK){
			cb_status = cbfifo_dequeue(TX_BUFFER, &tx_val);

			//Only proceed with transmission if circular buffer is not empty
			if(cb_status != CB_EMPTY && cb_status != CB_INSTANCE_ERROR)
			{
				UART0->D = tx_val;
				UART0->C2 &= ~UART0_C2_TIE(1); ///Disable transmit interrupt
			}
			else if(cb_status == CB_INSTANCE_ERROR){
				//Handle errors
			}
		}


	}

	__enable_irq();
}
/*-----------------------------------------------------------------------------------------------------------------------------*/
/*
 @brief: Implement sys_write functionality to link printf and UART
 @param: None
 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
int __sys_write(int handle, char *buf, int size)
{
	int status = 0;
	cb_error_status_e cb_status = CB_INSTANCE_SUCCESS;


	for(int i = 0; i < size ; i++)
	{
		cb_status = cbfifo_enqueue(TX_BUFFER, &buf[i]); //

		if(cb_status != CB_INSTANCE_SUCCESS)
		{
			status = -1;
			break;
		}

		UART0->C2 |= UART0_C2_TIE(1); //transmit interrupt enable

	}


	return status;
}
/*-----------------------------------------------------------------------------------------------------------------------------*/
/*
 @brief: Implement sys_read functionality to link getchar() and uart
 @param: None
 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
int __sys_readc(void)
{
	int status = 0;
	//if buffer is empty, then no need to dequeue
	cb_error_status_e cb_status = cbfifo_isempty(RX_BUFFER);

	if(cb_status == CB_EMPTY || cb_status == CB_INSTANCE_ERROR)
	{
		status = -1;
	}
	else
	{
		//Dequeue value from Rx circular buffer , return -1 on error and actual data byte on success
		cb_status = cbfifo_dequeue(RX_BUFFER, &status);

		if(cb_status == CB_INSTANCE_ERROR)
		{
			status = -1;
		}
	}

	return status;
}

/*-----------------------------------------------------------------------------------------------------------------------------*/
/*
 @brief: Initialise UART0 and UART1 module to 38400 and 9600 baud rate
 @param: None
 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
void uart_init()
{

	/******************************************************************
	 * 					UART0 INITIALISATION
	 ******************************************************************/
	uint16_t sbr;

	//Enable clock gating for UART0 and Port A
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

	//Disable transmitter and receiver
	UART0->C2 &= ~UART0_C2_TE_MASK & ~UART0_C2_RE_MASK;

	//set UART clock to  24 MHz clock
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1); //select clock source as PLL/2 or FLL
	SIM->SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK; //set FLL as clock source

	//set pins to UART0 Rx and Tx
	PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); //Rx
	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); //Tx

	//Set baud rate and oversampling ratio
	sbr = (uint16_t)((SYSCLOCK_FREQUENCY)/(BAUD_RATE * UART_OVERSAMPLE_RATE));
	UART0->BDH &= ~UART0_BDH_SBR_MASK;
	UART0->BDH |= UART0_BDH_SBR(sbr>>8);
	UART0->BDL = UART0_BDL_SBR(sbr);
	UART0->C4 |= UART0_C4_OSR(UART_OVERSAMPLE_RATE - 1);

	//Select two stop bit
	UART0->BDH |= UART0_BDH_SBNS(STOP_BITS);

	//Don't enable loopback mode, use 8 data bit mode, don't use parity
	UART0->C1 = UART0_C1_LOOPS(0) | UART0_C1_M(DATA_BITS) | UART0_C1_PE(PARITY);



	//send LSB first
	UART0->S2 = UART0_S2_MSBF(0);

	//Enable UART transmitter and receiver
	UART0->C2 |= UART0_C2_TE(1) | UART0_C2_RE(1);

	//Enable UART interrupt
	NVIC_EnableIRQ(UART0_IRQn);

	//Enable Tx and Rx interrupt
	UART0->C2 = 0;
	UART0->C2 |=  UART0_C2_RIE(1);

	/******************************************************************
	 * 					UART1 INITIALISATION(PTE1->Rx, PTE0->Tx)
	 ******************************************************************/
	//Enable clock gating for UART0 and Port A
	SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;
	UART1->C2 &= ~UART_C2_TE_MASK & ~UART_C2_RE_MASK;
//	UART2->BDH = 0x00;
//	UART2->BDL = 0x5B; /* 9600 Baud */

	sbr = (uint16_t)((SYSCLOCK_FREQUENCY)/(9600 * UART_OVERSAMPLE_RATE));
	UART1->BDH &= ~UART_BDH_SBR_MASK;
	UART1->BDH |= UART_BDH_SBR(sbr>>8);
	UART1->BDL = UART_BDL_SBR(sbr);
//	UART1->C4 |= UART_C4_OSR(UART_OVERSAMPLE_RATE - 1);

	UART1->C1 = 0x00; /* normal 8-bit, no parity */
	UART1->C3 = 0x00; /* no fault interrupt */
	UART1->C2 |= UART_C2_TE(1) | UART_C2_RE(1);
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; /* enable clock to PORTE */
	PORTE->PCR[1] = PORT_PCR_MUX(3); /* PTD5 for UART2 transmit */
	PORTE->PCR[0] = PORT_PCR_MUX(3); /* PTD5 for UART2 receive */

}

/*-----------------------------------------------------------------------------------------------------------------------------*/
/*
 @brief: Send message through UART1
 @param: None
 @return: None
 */
/*-------------------------------------------------------------------------*/
void uart1_puts(uint8_t* msg)
{
	uint8_t i = 0;
	while(msg[i] != '\0')
	{
		while(!(UART1->S1 & UART0_S1_TDRE_MASK)); /* wait for transmit buffer empty */
		UART2->D = msg[i];
		i++;
	}
}

