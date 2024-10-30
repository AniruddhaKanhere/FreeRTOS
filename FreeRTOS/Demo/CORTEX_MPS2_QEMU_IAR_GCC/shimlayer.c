#include "shimlayer.h"
#define RATE 100
#define BUFLEN 1024
#if( USING_MUTEX == 1 )
    int shim_send(uint8_t * data, size_t length)
    {
        static long bufferIndex = 0;
        static TickType_t lastIterationTickCount = 0;
        TickType_t currentTickCount = xTaskGetTickCount();
        TickType_t ticksSinceLastCall = currentTickCount - lastIterationTickCount;
        lastIterationTickCount = currentTickCount;
        // If there has been a tick this means time has passed in a measurable way, we simulate that the
        //    hardware has consumed from the buffer as many times as the ticks have counted.
        // If there has been no ticks since the last call all we have is the buffer, if there is space then
        //    great we accept the data, if not we accept what we have space left for, if any.
        if (ticksSinceLastCall > 0)
        {
            // Subtract number of ticks times rate from the buffer, if this is less than 0 we set the buffer to empty
            long maxConsumption = ticksSinceLastCall * RATE;
            if (maxConsumption >= bufferIndex) {
                bufferIndex = 0;
            } else {
                bufferIndex -= maxConsumption;
            }
        }
        // We allow at most the number of open spaces in the buffer to be sent
        long bytesAllowed = BUFLEN - bufferIndex;
        // If all the data fits in the buffer we send it all, else we send only the allowed amount.
        //   we indicate this by returning the number of bytes accepted into the buffer
        return bytesAllowed < length ? bytesAllowed : length;
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
