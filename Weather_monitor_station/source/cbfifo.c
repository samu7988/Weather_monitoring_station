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
#include "cbfifo.h"
#include "MKL25Z4.h"

//***********************************************************************************
//                                  Macros
//***********************************************************************************
//***********************************************************************************
//                              Structures
//***********************************************************************************


#define NUM_BYTES (256)


 static uint8_t tx_buffer[NUM_BYTES]; //statically allocated buffer for tx
 static uint8_t rx_buffer[NUM_BYTES]; //statically allocated buffer for rx


 cb_t cbfifo_handler[MAX_NUM_BUFFER] ; //Handler for circular buffer

//***********************************************************************************
//                                  Function definition
//***********************************************************************************

cb_error_status_e create_tx_cb_handle()
{
	cb_error_status_e status = 0;
	status = create_cb_instance(TX_BUFFER,tx_buffer, NUM_BYTES);

	return status;
}

cb_error_status_e create_rx_cb_handle()
{
	cb_error_status_e status = 0;
	status = create_cb_instance(RX_BUFFER,rx_buffer, NUM_BYTES);

	return status;
}
/*-----------------------------------------------------------------------------------------------------------------------------*/
/*
 @brief: To create the circular buffer
 @param: 1)cb_t* cbfifo_handler: Pointer to circular buffer handler
         2)uint8_t* cb_buffer: Pointer to statically allocated circular buffer
         3)size_t capacity: Capacity of circular buffer
 @return: CB_INSTANCE_SUCCESS on success, CB_INSTANCE_ERROR on failure
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/


cb_error_status_e create_cb_instance(buffer_type_e type,uint8_t* cb_buffer, size_t capacity){
    cb_error_status_e cb_status = CB_INSTANCE_SUCCESS;

    if(type >= MAX_NUM_BUFFER){
    	cb_status = CB_INSTANCE_ERROR;
    }
    else if(cb_buffer == NULL){
    	cb_status = CB_INSTANCE_ERROR;
    }
    else if(capacity == 0){
    	cb_status = CB_INSTANCE_ERROR;
    }
    else
    {
		cbfifo_handler[type].buffer = cb_buffer;
		cbfifo_handler[type].write = cbfifo_handler[type].buffer; //Point to starting location of CB
		cbfifo_handler[type].read = cbfifo_handler[type].buffer; //Points to start location of CB
		cbfifo_handler[type].length = 0;
		cbfifo_handler[type].capacity = capacity;

		cb_status = CB_INSTANCE_SUCCESS;
    }

    return cb_status;

}

/*-----------------------------------------------------------------------------------------------------------------------------*/
/*
 @brief: Check if circular buffer is empty
 @param: None
 @return: Returns 1 if empty , 0 if non empty
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/

cb_error_status_e cbfifo_isempty(buffer_type_e type){

	__disable_irq();

	cb_error_status_e status = CB_INSTANCE_SUCCESS;

	//Validate input
	if(type >= MAX_NUM_BUFFER)
		status = CB_INSTANCE_ERROR;

	else if(cbfifo_handler[type].length == 0)
        status = CB_EMPTY;

	__enable_irq();

    return status;
}

/*-----------------------------------------------------------------------------------------------------------------------------*/
/*
 @brief: Check if circular buffer is full
 @param: None
 @return: Returns 1 if empty , 0 if non full
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
cb_error_status_e cbfifo_isfull(buffer_type_e type){

	__disable_irq();

	cb_error_status_e status = CB_INSTANCE_SUCCESS;

	//Validate input
	if(type >= MAX_NUM_BUFFER)
		status = CB_INSTANCE_ERROR;

	else if(cbfifo_handler[type].length == cbfifo_handler[type].capacity)
        status =  CB_FULL;

	__enable_irq();

	return status;
}

/*-----------------------------------------------------------------------------------------------------------------------------*/
/*
 @brief: To Enque the circular buffer
 @param: 1)void *buf: Input buffer to be used for enqueing
         2)size_t nbyte: Number of bytes to be enqueued

 @return: Count of the Enqued data
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/

cb_error_status_e cbfifo_enqueue(buffer_type_e type, void *buf){

	__disable_irq();

	cb_error_status_e cb_status = CB_INSTANCE_SUCCESS;

    if(buf == NULL)
    	cb_status = CB_INSTANCE_ERROR ;

    if(cbfifo_handler[type].read == NULL)
    	cb_status = CB_INSTANCE_ERROR ;

    if(cbfifo_handler[type].write == NULL)
    	cb_status = CB_INSTANCE_ERROR ;

    if(cbfifo_handler[type].capacity == 0)
    	cb_status = CB_INSTANCE_ERROR ;

    if(cb_status != CB_INSTANCE_ERROR)
    {
		size_t count = 0;

		//if circular buffer is not full, enqueue data
		if(cbfifo_isfull(type) != CB_FULL){

			*(cbfifo_handler[type].write) = *((uint8_t*)buf);


			cbfifo_handler[type].length = cbfifo_handler[type].length + 1;

			//Wrap around to start if write pointer reachers end of buffer
			if(cbfifo_handler[type].write == cbfifo_handler[type].buffer + cbfifo_handler[type].capacity - 1){
				cbfifo_handler[type].write = cbfifo_handler[type].buffer;
			}
			else{
				cbfifo_handler[type].write = cbfifo_handler[type].write + 1;
			}
			count = count + 1;

		}
		else{
			//if buffer is full, break from for loop
			cb_status = CB_FULL; ;

		}
    }
	__enable_irq();

	return cb_status;

}
/*-----------------------------------------------------------------------------------------------------------------------------*/
/*
 @brief: To Deque the circular buffer
 @param: 1)void *buf: Put dequeued data into this buffer
         2)size_t nbyte: Number of bytes to be dequeued

 @return: Returns Count of the Dequed data
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
cb_error_status_e cbfifo_dequeue(buffer_type_e type, void *buf){   //*buff is pointing to the recieving buffer

	__disable_irq();

	cb_error_status_e cb_status = CB_INSTANCE_SUCCESS;

	//input validate
    if(buf == NULL){
        cb_status = CB_INSTANCE_ERROR;
    }
    if(cbfifo_handler[type].read == NULL)
        cb_status = CB_INSTANCE_ERROR;
    if(cbfifo_handler[type].write == NULL)
        cb_status = CB_INSTANCE_ERROR;
    if(cbfifo_handler[type].capacity == 0)
        cb_status = CB_INSTANCE_ERROR;

    size_t count = 0;
    if(cb_status != CB_INSTANCE_ERROR)
    {
		//if CB is not empty
		if(cbfifo_isempty(type) != CB_EMPTY){

			*((uint8_t*)buf) = *(cbfifo_handler[type].read); //Read data from read pointer
			*(cbfifo_handler[type].read) = 0;
			cbfifo_handler[type].length = cbfifo_handler[type].length - 1;
			if(cbfifo_handler[type].read == (cbfifo_handler[type].buffer + cbfifo_handler[type].capacity - 1)){
				cbfifo_handler[type].read = cbfifo_handler[type].buffer;
			}
			else{
				cbfifo_handler[type].read = cbfifo_handler[type].read + 1;
			}
			count = count + 1;

		}
		else{
			//if CB is empty
			cb_status = CB_EMPTY;
		}
    }
	__enable_irq();

	return cb_status;
}
/*-----------------------------------------------------------------------------------------------------------------------------*/
/*
 @brief: To get the length of the enqued data in circukar buffer
 @param: None
 @return:Returns number of Enqued data
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
size_t cbfifo_length(buffer_type_e type){

	__disable_irq();

	size_t status = 0;

	if(type >= MAX_NUM_BUFFER)
		status =  CB_INSTANCE_ERROR;
	else
		status = cbfifo_handler[type].length;

	__enable_irq();

	return status;
}

/*-----------------------------------------------------------------------------------------------------------------------------*/
/*
 @brief: To get capacity of the circular buffer
 @param: None


 @return:Returns the capacity of the circular buffer
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/

size_t cbfifo_capacity(buffer_type_e type){

	__disable_irq();

	size_t status = 0;
	if(type >= MAX_NUM_BUFFER)
		status =  CB_INSTANCE_ERROR;
	else
		status = cbfifo_handler[type].capacity;

	__enable_irq();

	return status;
}
