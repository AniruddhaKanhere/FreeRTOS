#ifndef _PRODUCER_H_
#define _PRODUCER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

typedef struct producerTaskParams{
    TickType_t delayBetweenIteration;
    size_t bytesToSendPerIteration;
    size_t impulse_magnitude;
} producerTaskParams_t;


void initProducer();
void producerTask( void * params );


#endif