#include "shimlayer.h"
#include "consumer.h"


// CVE - What was the purpose of this shim layer? The interface can be defined in a header, I would prefer that, define the interface for send and recv 
//   and implement it like a library. Define the functions as extern so the linker resolves them. Best case we can compile the entire test framework
//   into a ".a" and you can just link it with the DUT code.


#if( USING_MUTEX == 1 )
    int shim_send(uint8_t * data, size_t length)
    {
        return sendAtRate(data, length);
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
