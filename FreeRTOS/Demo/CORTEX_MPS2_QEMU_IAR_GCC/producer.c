#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "producer.h"
#include "shimlayer.h"

#define PRODUCER_IMPULSE_TICK_COUNT  5000
#define PRODUCER_IMPULSE_MAGNITUDE   1000000  // 1 Million bytes to send !
#define PRODUCER_BUFFER_SIZE         2000


static uint8_t producerBuffer[PRODUCER_BUFFER_SIZE] = {};

// The produceer should implement an impulse response to the input, which is "data to be sent". If the data goes from 0 to X in an instant the system
//    impulse response, which we are measuring, will be sufficient to completely characterize the system. The data may be chunked, by making multiple 
//    calls to the underlying send function, or it can be sent in a single call. 
void producerTask( void * params )
{
    producerTaskParams_t * producerTaskParams = (producerTaskParams_t *)params;
    
    size_t remainingBytes = PRODUCER_IMPULSE_MAGNITUDE;
    
    // Fill the buffer before we start the timers, we will send the same chunk of data over and over to remove production of the data from the 
    //   measurement of effort for the test.
    for (int i=0; i<PRODUCER_BUFFER_SIZE; i++)
    {
        producerBuffer[i] = i & 0x000000FF;
    }

    // Wait for the trigger time to pass, basically hit the stopwatch when we hit PRODUCER_IMPULSE_TICK_COUNT
    while(PRODUCER_IMPULSE_TICK_COUNT > xTaskGetTickCount());

    // Send a full chunk if we can
    while(remainingBytes > producerTaskParams->bytesToSendPerIteration)
    {
        shim_send(producerBuffer, producerTaskParams->bytesToSendPerIteration);
        remainingBytes -= producerTaskParams->bytesToSendPerIteration;

        if(producerTaskParams->delayBetweenIteration != portMAX_DELAY)
        {
            vTaskDelay(producerTaskParams->delayBetweenIteration);
        }

        //printf(".");
    }

    if (remainingBytes > 0)
    {
        shim_send(producerBuffer, producerTaskParams->bytesToSendPerIteration);
    }

    TickType_t endTime = xTaskGetTickCount();

    printf("\r\nCompleted in %d ticks\r\n", endTime - PRODUCER_IMPULSE_TICK_COUNT);

    // Wait for better days here.
    while(1);
}