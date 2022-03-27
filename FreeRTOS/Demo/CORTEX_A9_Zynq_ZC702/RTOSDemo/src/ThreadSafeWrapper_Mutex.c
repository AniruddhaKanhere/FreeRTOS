/*
 * ThreadSafeWrapper_Mutex.c
 *
 *  Created on: Mar 23, 2022
 *      Author: kanherea
 */

#include "FreeRTOS.h"
#include "semphr.h"

#include "NonThreadSafeAPI.h"
#include "ThreadSafeWrapper_Mutex.h"

static SemaphoreHandle_t ThreadSafeMutex = NULL;
static BaseType_t xMutexInit = 0;

BaseType_t ThreadSafeWrapper_MutexInit( void )
{
	BaseType_t xReturn = pdFAIL;

	if( xMutexInit != pdTRUE )
	{
	    ThreadSafeMutex = xSemaphoreCreateMutex();

	    if( ThreadSafeMutex != NULL )
	    {
		    xReturn = pdPASS;
		    xMutexInit = pdTRUE;
	    }
	}

	return xReturn;
}

BaseType_t ThreadSafeWrapper_MutexSend( UBaseType_t uxValueToSend,
		                                UBaseType_t uxUseBusyWait,
		                                BaseType_t xUseRandomValues,
		                                TickType_t uxTimeout )
{
	BaseType_t xReturn = pdFAIL;

	configASSERT( ThreadSafeMutex != NULL );

	if( xSemaphoreTake( ThreadSafeMutex, uxTimeout ) == pdTRUE )
	{
	    NonThreadSafeAPI( uxValueToSend,
                          uxUseBusyWait,
                          xUseRandomValues );

	    ( void ) xSemaphoreGive( ThreadSafeMutex );

	    xReturn = pdPASS;
	}


	return xReturn;
}

void ThreadSafeWrapper_MutexDeInit( void )
{
	if( xMutexInit == pdTRUE )
	{
		vSemaphoreDelete( ThreadSafeMutex );
		xMutexInit = pdFALSE;
	}
}
