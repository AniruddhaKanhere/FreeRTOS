#ifndef _SHIMLAYER_H_
#define _SHIMLAYER_H_

#include <stdint.h>
#include <stddef.h>
#include "FreeRTOS.h"

#define USING_CRITICAL_SECTION ( 0 )
#define USING_MUTEX       ( 1 )
#define USING_SEMAPHORE   ( 0 )
#define USING_QUEUE       ( 0 )

#if ( ( USING_CRITICAL_SECTION + USING_MUTEX + USING_SEMAPHORE + USING_QUEUE ) != 1U )
    #error "Only one of the following can be defined: USING_CRITICAL_SECTION, USING_MUTEX, USING_SEMAPHORE, USING_QUEUE
#endif

int shim_send( uint8_t * arr, size_t len );
size_t shim_recv(uint8_t * arr, size_t len);

#endif