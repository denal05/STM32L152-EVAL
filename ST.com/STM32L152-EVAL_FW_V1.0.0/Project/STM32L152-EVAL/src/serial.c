/*
    FreeRTOS V7.0.2 - Copyright (C) 2011 Real Time Engineers Ltd.
	

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/*
	BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR UART0.
	
	***Note*** This example uses queues to send each character into an interrupt
	service routine and out of an interrupt service routine individually.  This
	is done to demonstrate queues being used in an interrupt, and to deliberately
	load the system to test the FreeRTOS port.  It is *NOT* meant to be an 
	example of an efficient implementation.  An efficient implementation should
	use FIFO's or DMA if available, and only use FreeRTOS API functions when 
	enough has been received to warrant a task being unblocked to process the
	data.
*/

/* Scheduler includes. */
/*
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "comtest2.h"
*/

/* Library includes. */
#include "stm32l152_eval.h"

/* Demo application includes. */
#include "serial.h"
/*-----------------------------------------------------------*/

/* Misc defines. */
#define serINVALID_QUEUE				( ( xQueueHandle ) 0 )
#define serNO_BLOCK						( ( portTickType ) 0 )

/*-----------------------------------------------------------*/

#define portCHAR        char
#define portFLOAT       float
#define portDOUBLE      double
#define portLONG        int
#define portSHORT       short
#define portSTACK_TYPE  unsigned int
#define portBASE_TYPE	int
#define portTickType    unsigned int

#define queueUNLOCKED					( ( signed portBASE_TYPE ) -1 )

#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5

#define pdTRUE		( 1 )
#define pdFALSE		( 0 )
#define pdPASS									( 1 )
#define pdFAIL									( 0 )

/*
 * Definition of the only type of object that a list can contain.
 */
struct xLIST_ITEM
{
	portTickType xItemValue;				/*< The value being listed.  In most cases this is used to sort the list in descending order. */
	volatile struct xLIST_ITEM * pxNext;	/*< Pointer to the next xListItem in the list. */
	volatile struct xLIST_ITEM * pxPrevious;/*< Pointer to the previous xListItem in the list. */
	void * pvOwner;							/*< Pointer to the object (normally a TCB) that contains the list item.  There is therefore a two way link between the object containing the list item and the list item itself. */
	void * pvContainer;						/*< Pointer to the list in which this list item is placed (if any). */
};
typedef struct xLIST_ITEM xListItem;		/* For some reason lint wants this as two separate definitions. */

struct xMINI_LIST_ITEM
{
	portTickType xItemValue;
	volatile struct xLIST_ITEM *pxNext;
	volatile struct xLIST_ITEM *pxPrevious;
};
typedef struct xMINI_LIST_ITEM xMiniListItem;

/*
 * Definition of the type of queue used by the scheduler.
 */
typedef struct xLIST
{
	volatile unsigned portBASE_TYPE uxNumberOfItems;
	volatile xListItem * pxIndex;			/*< Used to walk through the list.  Points to the last item returned by a call to pvListGetOwnerOfNextEntry (). */
	volatile xMiniListItem xListEnd;		/*< List item that contains the maximum possible item value meaning it is always at the end of the list and is therefore used as a marker. */
} xList;

/*
 * Definition of the queue used by the scheduler.
 * Items are queued by copy, not reference.
 */
typedef struct QueueDefinition
{
	signed char *pcHead;				/*< Points to the beginning of the queue storage area. */
	signed char *pcTail;				/*< Points to the byte at the end of the queue storage area.  Once more byte is allocated than necessary to store the queue items, this is used as a marker. */

	signed char *pcWriteTo;				/*< Points to the free next place in the storage area. */
	signed char *pcReadFrom;			/*< Points to the last place that a queued item was read from. */

	xList xTasksWaitingToSend;				/*< List of tasks that are blocked waiting to post onto this queue.  Stored in priority order. */
	xList xTasksWaitingToReceive;			/*< List of tasks that are blocked waiting to read from this queue.  Stored in priority order. */

	volatile unsigned portBASE_TYPE uxMessagesWaiting;/*< The number of items currently in the queue. */
	unsigned portBASE_TYPE uxLength;		/*< The length of the queue defined as the number of items it will hold, not the number of bytes. */
	unsigned portBASE_TYPE uxItemSize;		/*< The size of each items that the queue will hold. */

	signed portBASE_TYPE xRxLock;			/*< Stores the number of items received from the queue (removed from the queue) while the queue was locked.  Set to queueUNLOCKED when the queue is not locked. */
	signed portBASE_TYPE xTxLock;			/*< Stores the number of items transmitted to the queue (added to the queue) while the queue was locked.  Set to queueUNLOCKED when the queue is not locked. */

} xQUEUE;
/*-----------------------------------------------------------*/

/*
 * Inside this file xQueueHandle is a pointer to a xQUEUE structure.
 * To keep the definition private the API header file defines it as a
 * pointer to void.
 */
typedef xQUEUE * xQueueHandle;

/* The queue used to hold received characters. */
static xQueueHandle xRxedChars;
static xQueueHandle xCharsForTx;

#if !defined(_SIZE_T) && !defined(_SIZET)
  #define _SIZE_T
  #define _SIZET
  #define _STD_USING_SIZE_T
typedef _Sizet size_t;
#endif /* !defined(_SIZE_T) && !defined(_SIZET) */

#ifndef NULL
  #define NULL   _NULL
#endif /* NULL */

/*-----------------------------------------------------------*/

signed portBASE_TYPE xQueueReceiveFromISR( xQueueHandle pxQueue, void * const pvBuffer, signed portBASE_TYPE *pxTaskWoken )
{
signed portBASE_TYPE xReturn;
unsigned portBASE_TYPE uxSavedInterruptStatus;

	configASSERT( pxQueue );
	configASSERT( pxTaskWoken );
	configASSERT( !( ( pvBuffer == NULL ) && ( pxQueue->uxItemSize != ( unsigned portBASE_TYPE ) 0U ) ) );

	uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
	{
		/* We cannot block from an ISR, so check there is data available. */
		if( pxQueue->uxMessagesWaiting > ( unsigned portBASE_TYPE ) 0 )
		{
			traceQUEUE_RECEIVE_FROM_ISR( pxQueue );

			prvCopyDataFromQueue( pxQueue, pvBuffer );
			--( pxQueue->uxMessagesWaiting );

			/* If the queue is locked we will not modify the event list.  Instead
			we update the lock count so the task that unlocks the queue will know
			that an ISR has removed data while the queue was locked. */
			if( pxQueue->xRxLock == queueUNLOCKED )
			{
				if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
				{
					if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) != pdFALSE )
					{
						/* The task waiting has a higher priority than us so
						force a context switch. */
						*pxTaskWoken = pdTRUE;
					}
				}
			}
			else
			{
				/* Increment the lock count so the task that unlocks the queue
				knows that data was removed while it was locked. */
				++( pxQueue->xRxLock );
			}

			xReturn = pdPASS;
		}
		else
		{
			xReturn = pdFAIL;
			traceQUEUE_RECEIVE_FROM_ISR_FAILED( pxQueue );
		}
	}
	portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

	return xReturn;
}
/*-----------------------------------------------------------*/



#define xQueueSendFromISR( pxQueue, pvItemToQueue, pxHigherPriorityTaskWoken ) xQueueGenericSendFromISR( ( pxQueue ), ( pvItemToQueue ), ( pxHigherPriorityTaskWoken ), queueSEND_TO_BACK )

/*-----------------------------------------------------------
 * PUBLIC QUEUE MANAGEMENT API documented in queue.h
 *----------------------------------------------------------*/

xQueueHandle xQueueCreate( unsigned portBASE_TYPE uxQueueLength, unsigned portBASE_TYPE uxItemSize )
{
xQUEUE *pxNewQueue;
size_t xQueueSizeInBytes;
xQueueHandle xReturn = NULL;

	/* Allocate the new queue structure. */
	if( uxQueueLength > ( unsigned portBASE_TYPE ) 0 )
	{
		pxNewQueue = ( xQUEUE * ) pvPortMalloc( sizeof( xQUEUE ) );
		if( pxNewQueue != NULL )
		{
			/* Create the list of pointers to queue items.  The queue is one byte
			longer than asked for to make wrap checking easier/faster. */
			xQueueSizeInBytes = ( size_t ) ( uxQueueLength * uxItemSize ) + ( size_t ) 1;

			pxNewQueue->pcHead = ( signed char * ) pvPortMalloc( xQueueSizeInBytes );
			if( pxNewQueue->pcHead != NULL )
			{
				/* Initialise the queue members as described above where the
				queue type is defined. */
				pxNewQueue->pcTail = pxNewQueue->pcHead + ( uxQueueLength * uxItemSize );
				pxNewQueue->uxMessagesWaiting = ( unsigned portBASE_TYPE ) 0U;
				pxNewQueue->pcWriteTo = pxNewQueue->pcHead;
				pxNewQueue->pcReadFrom = pxNewQueue->pcHead + ( ( uxQueueLength - ( unsigned portBASE_TYPE ) 1U ) * uxItemSize );
				pxNewQueue->uxLength = uxQueueLength;
				pxNewQueue->uxItemSize = uxItemSize;
				pxNewQueue->xRxLock = queueUNLOCKED;
				pxNewQueue->xTxLock = queueUNLOCKED;

				/* Likewise ensure the event queues start with the correct state. */
				vListInitialise( &( pxNewQueue->xTasksWaitingToSend ) );
				vListInitialise( &( pxNewQueue->xTasksWaitingToReceive ) );

				traceQUEUE_CREATE( pxNewQueue );
				xReturn = pxNewQueue;
			}
			else
			{
				traceQUEUE_CREATE_FAILED();
				vPortFree( pxNewQueue );
			}
		}
	}

	configASSERT( xReturn );

	return xReturn;
}

/*-----------------------------------------------------------*/

/*
 * See the serial2.h header file.
 */
xComPortHandle xSerialPortInitMinimal( unsigned long ulWantedBaud, unsigned int uxQueueLength )
{
USART_InitTypeDef USART_InitStructure;
xComPortHandle xReturn;
NVIC_InitTypeDef NVIC_InitStructure;

	/* Create the queues used to hold Rx/Tx characters. */
	/// xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	/// xCharsForTx = xQueueCreate( uxQueueLength + 1, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );

	/* If the queues were created correctly then setup the serial port
	hardware. */
	/// if( ( xRxedChars != serINVALID_QUEUE ) && ( xCharsForTx != serINVALID_QUEUE ) )
	/// {
		USART_InitStructure.USART_BaudRate = ulWantedBaud;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		
		/* The Eval board COM2 is being used, which in reality is the STM32
		USART3. */
		STM_EVAL_COMInit( COM2, &USART_InitStructure );
		
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; /* Not used as 4 bits are used for the pre-emption priority. */;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init( &NVIC_InitStructure );
		USART_ITConfig( USART3, USART_IT_RXNE, ENABLE );
	/// }
	/// else
	/// {
	/// 	xReturn = ( xComPortHandle ) 0;
	/// }

	/* This demo file only supports a single port but we have to return
	something to comply with the standard demo header file. */
	return xReturn;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed portCHAR *pcRxedChar, portTickType xBlockTime )
{
	/* The port handle is not required as this driver only supports one port. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed portCHAR cOutChar, portTickType xBlockTime )
{
signed portBASE_TYPE xReturn;

	if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) == pdPASS )
	{
		xReturn = pdPASS;
		USART_ITConfig( USART3, USART_IT_TXE, ENABLE );
	}
	else
	{
		xReturn = pdFAIL;
	}

	return xReturn;
}

void vSerialPutString( xComPortHandle pxPort, const signed portCHAR * const pcString, unsigned portSHORT usStringLength )
{
signed portCHAR *pxNext;

	/* A couple of parameters that this port does not use. */
	( void ) usStringLength;
	( void ) pxPort;

	/* NOTE: This implementation does not handle the queue being full as no
	block time is used! */

	/* The port handle is not required as this driver only supports UART1. */
	( void ) pxPort;

	/* Send each character in the string, one at a time. */
	pxNext = ( signed portCHAR * ) pcString;
	while( *pxNext )
	{
		xSerialPutChar( pxPort, *pxNext, serNO_BLOCK );
		pxNext++;
	}
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

void vSerialClose( xComPortHandle xPort )
{
	/* Not supported as not required by the demo application. */
}
/*-----------------------------------------------------------*/

void USART3_IRQHandler( void )
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
portCHAR cChar;

	if( USART_GetITStatus( USART3, USART_IT_TXE ) == SET )
	{
		/* The interrupt was caused by the TX register becoming empty.  Are 
		there any more characters to transmit? */
		if( xQueueReceiveFromISR( xCharsForTx, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE )
		{
			/* A character was retrieved from the queue so can be sent to the
			USART now. */
			USART_SendData( USART3, cChar );
		}
		else
		{
			USART_ITConfig( USART3, USART_IT_TXE, DISABLE );		
		}		
	}
	
	if( USART_GetITStatus( USART3, USART_IT_RXNE ) == SET )
	{
		/* A character has been received on the USART, send it to the Rx
		handler task. */
		cChar = USART_ReceiveData( USART3 );
		xQueueSendFromISR( xRxedChars, &cChar, &xHigherPriorityTaskWoken );
	}	

	/* If sending or receiving from a queue has caused a task to unblock, and
	the unblocked task has a priority equal to or higher than the currently 
	running task (the task this ISR interrupted), then xHigherPriorityTaskWoken 
	will have automatically been set to pdTRUE within the queue send or receive 
	function.  portEND_SWITCHING_ISR() will then ensure that this ISR returns 
	directly to the higher priority unblocked task. */
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}





	
