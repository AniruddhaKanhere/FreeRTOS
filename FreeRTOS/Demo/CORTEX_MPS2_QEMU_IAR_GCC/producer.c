#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "producer.h"
#include "shimlayer.h"

#include <inttypes.h>


#define PRODUCER_IMPULSE_TICK_COUNT  1000
#define PRODUCER_BUFFER_SIZE         10240


static uint8_t producerBuffer[PRODUCER_BUFFER_SIZE] = {};

/* 
   This producer task function will busy wait until the prescribed tick count arrives, and then it will immediately attempt to send
      the presecribed number of bytes in the prescribed chunk size as fast as possible, after which it will delete itself, freeing the 
      memory it was consuming. 
*/

void initProducer()
{
    // Fill the buffer before we start the timers, we will send the same chunk of data over and over to remove production of the data from the 
    //   measurement of effort for the test.
    for (int i=0; i<PRODUCER_BUFFER_SIZE; i++)
    {
        producerBuffer[i] = i & 0x000000FF;
    }
}


// The produceer should implement an impulse response to the input, which is "data to be sent". If the data goes from 0 to X in an instant the system
//    impulse response, which we are measuring, will be sufficient to completely characterize the system. The data may be chunked, by making multiple 
//    calls to the underlying send function, or it can be sent in a single call. 
void producerTask( void * params )
{
    producerTaskParams_t * producerTaskParams = (producerTaskParams_t *)params;
    
    size_t remainingBytes = producerTaskParams->impulse_magnitude;

    // Wait for the trigger time to pass, basically hit the stopwatch when we hit PRODUCER_IMPULSE_TICK_COUNT
    while(PRODUCER_IMPULSE_TICK_COUNT > xTaskGetTickCount());

    // Calk the chunk size
    size_t chunksize = (PRODUCER_BUFFER_SIZE < producerTaskParams->bytesToSendPerIteration)? PRODUCER_BUFFER_SIZE : producerTaskParams->bytesToSendPerIteration;

    // Send a full chunk if we can
    while(remainingBytes > chunksize)
    {
        size_t actualBytesSent = shim_send(producerBuffer, chunksize);
        remainingBytes -= actualBytesSent;

        if(producerTaskParams->delayBetweenIteration != portMAX_DELAY)
        {
            //vTaskDelay(producerTaskParams->delayBetweenIteration);
        }

        //printf(".");
    }

    // Send the last/partial chunk if something is left, the buffer may be full so we need to keep going until it is all sent !
    while (remainingBytes > 0)
    {
        remainingBytes -= shim_send(producerBuffer, remainingBytes);
    }

    TickType_t endTime = xTaskGetTickCount();

    printf("\r\nCompleted in %u ticks\r\n", (int)(endTime - PRODUCER_IMPULSE_TICK_COUNT));

    // This task will delete itself when done
    vTaskDelete(NULL);
}