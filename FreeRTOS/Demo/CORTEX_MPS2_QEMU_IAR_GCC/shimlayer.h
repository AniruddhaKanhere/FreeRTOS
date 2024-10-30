#ifndef _SHIMLAYER_H_
#define _SHIMLAYER_H_

#include <stdint.h>
#include <stddef.h>
#include "FreeRTOS.h"

#define USING_MUTEX       ( 1 )
#define USING_SEMAPHORE   ( 0 )
#define USING_QUEUE       ( 0 )

int shim_send( uint8_t * arr, size_t len );
size_t shim_recv(uint8_t * arr, size_t len);

#endif