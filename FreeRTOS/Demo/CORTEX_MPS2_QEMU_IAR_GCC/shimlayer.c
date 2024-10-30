#include "shimlayer.h"
#include "consumer.h"

#define RATE 100
#define BUFLEN 1024
#if( USING_MUTEX == 1 )
    int shim_send(uint8_t * data, size_t length)
    {
        return sendAtRate(data, length);
    }

    size_t shim_recv(uint8_t * arr, size_t len)
    {
        // Receive from dump directly.
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
