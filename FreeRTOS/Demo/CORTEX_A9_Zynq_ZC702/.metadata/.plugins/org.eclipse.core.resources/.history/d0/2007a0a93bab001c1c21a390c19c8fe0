/*
 * NonThreadSafeAPI.c
 *
 *  Created on: Mar 23, 2022
 *      Author: kanherea
 */

#include "FreeRTOS.h"
#include "task.h"

#include "NonThreadSafeAPI.h"

#ifndef MAX_RND_VALUE
    #define MAX_RND_VALUE    0xFFFFFFFF
#endif

uint32_t seed = 123456789;
uint32_t m = 65537; /* (2^16) +1. */
uint32_t a = 75;
uint32_t c = 74;

uint32_t rand( void )
{
	seed = (a * seed + c) % m;

	return ( seed % MAX_RND_VALUE );
}

void NonThreadSafeAPI( UBaseType_t uxCount,
		               UBaseType_t uxUseBusyWait,
					   BaseType_t xUseRandomValues )
{
	/* Declare the variable volatile so that the busy loop
	 * doesn't get optimized away. */
	volatile UBaseType_t uxCounter;
	UBaseType_t uxLocalCount;

	if( xUseRandomValues != pdTRUE )
	{
		uxLocalCount = uxCount;
	}
	else
	{
		uxLocalCount = rand();
	}

	if( uxUseBusyWait == 0 )
	{
	    vTaskDelay( pdMS_TO_TICKS( uxLocalCount ) );
	}
	else
	{
        for( uxCounter = 0; uxCounter < uxLocalCount;  )
        {
        	/* Busy wait. */
        	uxCounter++;
        }
	}
}
