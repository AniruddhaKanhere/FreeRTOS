#include "shimlayer.h"

#if( USING_MUTEX == 1 )
    size_t shim_send( uint8_t * arr, size_t len )
    {

        return consume(arr,len);
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
