#include "shimlayer.h"
#include "consumer.h"

#include "task.h"
#include "queue.h"
#include "semphr.h"

SemaphoreHandle_t xSemaphore = NULL;

// CVE - What was the purpose of this shim layer? The interface can be defined in a header, I would prefer that, define the interface for send and recv 
//   and implement it like a library. Define the functions as extern so the linker resolves them. Best case we can compile the entire test framework
//   into a ".a" and you can just link it with the DUT code.

// Ok in retrospect I get it, this is actually the DUP implementation, it is called via the shim_send interface and it calls the sendAtRate interface on 
//   the other side. Lets figure out how we make this clean

#if( USING_CRITICAL_SECTION == 1 )
    int shim_send(uint8_t * data, size_t length)
    {
        int retVal;
        taskENTER_CRITICAL();
        retVal = sendAtRate(data, length);
        taskEXIT_CRITICAL();
        return retVal;
    }

    size_t shim_recv(uint8_t * arr, size_t len)
    {
        // Receive from dump directly.
        return 0;
    }
#elif( USING_MUTEX == 1 )
    int shim_send(uint8_t * data, size_t length)
    {
        int retVal;
        xSemaphoreTake( xSemaphore, ( TickType_t ) portMAX_DELAY );
        retVal = sendAtRate(data, length);
        xSemaphoreGive( xSemaphore );
        return retVal;
    }

    size_t shim_recv(uint8_t * arr, size_t len)
    {
        // Receive from dump directly.
        return 0;
    }
#elif( USING_QUEUE == 1 )
    size_t shim_send(uint8_t * arr, size_t len)
    {
        // Send to queue.
    }
    size_t shim_recv(uint8_t * arr, size_t len)
    {
        // Receive from queue.
    }
#endif
