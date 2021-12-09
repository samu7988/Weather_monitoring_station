/***********************************************************************************
* @file   cbfifo.h
 * @brief:A fixed-size FIFO implemented via a circular buffer
 *        1)operation on circular buffer
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

#ifndef _CBFIFO_H_
#define _CBFIFO_H_

//***********************************************************************************
//                              Include files
//***********************************************************************************
#include <stdlib.h>  // for size_t
#include <stdint.h>

//***********************************************************************************
//                              Macros
//***********************************************************************************
typedef enum
{
	CB_INSTANCE_ERROR = -1,
	CB_INSTANCE_SUCCESS = 0,
	CB_FULL,
	CB_EMPTY
}cb_error_status_e;

typedef struct{
    uint8_t *buffer;
    uint8_t* write;
    uint8_t* read;
    size_t length;
    size_t capacity;
}cb_t;


typedef enum{
	TX_BUFFER = 0,
	RX_BUFFER = 1,
	TEST_BUFFER = 2,
	MAX_NUM_BUFFER = 3
}buffer_type_e;
//***********************************************************************************
//                              Global variables
//***********************************************************************************

extern cb_t cbfifo_handler[MAX_NUM_BUFFER]; //Handler for circular buffer

/*------------------------------------------------------------------------------------------------------------------------------------*/
/*
 @brief: To create the circular buffer
 @param: 1)buffer_type_e
         2)uint8_t* cb_buffer
         3)size_t capacity
 @return: CB_INSTANCE_SUCCESS
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
cb_error_status_e create_cb_instance(buffer_type_e type,uint8_t* cb_buffer, size_t capacity);

/*------------------------------------------------------------------------------------------------------------------------------------*/
 /*
 @brief: Enqueues data onto the FIFO, up to the limit of the available FIFO
        capacity.
 @param: 1)void *buf    - Pointer to the data

 @return: The number of bytes actually enqueued, which could be 0. In case
 *        of an error, returns -1.
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/

cb_error_status_e cbfifo_enqueue(buffer_type_e type, void *buf);
/*------------------------------------------------------------------------------------------------------------------------------------*/
/*
 @brief:* Attempts to remove ("dequeue") up to nbyte bytes of data from the
 *        FIFO. Removed data will be copied into the buffer pointed to by buf.
 @param: 1)void *buf    - Destination for the dequeued data

 @return:  The number of bytes actually copied, which will be between 0 and
 *          nbyte.
           To further explain the behavior: If the FIFO's current length is 24
 *         bytes, and the caller requests 30 bytes, cbfifo_dequeue should
 *         return the 24 bytes it has, and the new FIFO length will be 0. If
 *         the FIFO is empty (current length is 0 bytes), a request to dequeue
 *         any number of bytes will result in a return of 0 from
 *         cbfifo_dequeue.
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/

cb_error_status_e cbfifo_dequeue(buffer_type_e type, void *buf);
/*------------------------------------------------------------------------------------------------------------------------------------*/
/*
 @brief: Returns the number of bytes currently on the FIFO.
 @param: None
 @return:Number of bytes currently available to be dequeued from the FIFO
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
size_t cbfifo_length(buffer_type_e type);

/*
 @brief: the FIFO's capacity
 @param: None
 @return:The capacity, in bytes, for the FIFO
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
size_t cbfifo_capacity(buffer_type_e type);

cb_error_status_e create_tx_cb_handle();
cb_error_status_e create_rx_cb_handle();
cb_error_status_e cbfifo_isempty(buffer_type_e type);



#endif // _CBFIFO_H_
