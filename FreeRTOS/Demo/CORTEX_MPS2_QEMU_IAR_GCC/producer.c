#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "producer.h"
#include "shimlayer.h"


void producerTask( void * params )
{
    producerTaskParams_t * producerTaskParams = (producerTaskParams_t *)params;
    uint8_t array[2000];
    size_t index = 0;

    while(1)
    {
        size_t bytesToSend = (producerTaskParams->bytesToSendPerIteration > (sizeof(array) - index))?
                                    (sizeof(array) - index) : producerTaskParams->bytesToSendPerIteration;

        shim_send(array[index], bytesToSend);
        index += bytesToSend;

        if(producerTaskParams->delayBetweenIteration != portMAX_DELAY)
        {
            vTaskDelay(producerTaskParams->delayBetweenIteration);
        }
    }
}