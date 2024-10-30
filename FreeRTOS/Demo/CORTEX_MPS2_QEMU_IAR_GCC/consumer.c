#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "FreeRTOS.h"

// Rate in bytes per tick
// Limitation : This does not deal with the tick counter wrapping
#define RATE  100
#define BUFLEN 1000

int sendAtRate(void * data, int length)
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