#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "FreeRTOS.h"
#include "task.h"

// Limitation : This does not (yet) deal with the tick counter wrapping. Logic to deal with that would be too expensive and we can do a
//                perfectly sufficient test without running so long that the counter wraps, so when using this avoid this condition.

// Rate in bytes per tick
#define RATE   100
#define BUFLEN 1000

#if BUFLEN < RATE*2
    #warning If the configured buffer length is less than 2x the data rate this model will suffer from timing inaccuracies 
#endif

// This simulates the entry of the hardware buffer and should be volatile to avoid the copying of date being optimized out
static volatile uint8_t fakeSendRegister;

/*
    This function will model a hardware peripheral which has a configurable sized hardware buffer and a configurable data rate.

    The model has 2 state variables, the index indicating which data in the buffer has not yet been consumed and the last tick count when
       this function was called. 
       
    When entering we adjust the buffer state through calculation to what it would have been if the HW peripheral had been consuming the data at 
       the configured rate. Since the timing is accurate only to the resolution of 1 tick accuracy below 1 tick will be reduced in the case where
       the buffer size is less than what is required to store data for 2 ticks. As long as the buffer size exceeds the tick rate x2 the consumption
       rate for the function will be 100% accurate. 

    This model assumes that you will be sending data during every tick, if you have lots of high priority tasks not sending data by calling this 
       function you may have to extend the buffer size to 1 more than the longest time between calls to this function in order to achieve maximum 
       data transfer rate, if you call the function less than this the max data rate cannot be achieved.
*/
int sendAtRate(void * data, int length)
{
    /* Buffer index increments when there are bytes in the buffer, 0 indicates the buffer is empty */
    static long bufferIndex = 0;
    static TickType_t lastIterationTickCount = 0;

    // TODO: this bit here should be de-FreeRTOS-ified, so that we do not use TickType_t and we do not use the Kernel specific function to get
    //          the tick count. Add an abstracted interface to get the tick count, perhaps a Macro through config to do it most effeciently.
    TickType_t currentTickCount = xTaskGetTickCount();
    TickType_t ticksSinceLastCall = currentTickCount - lastIterationTickCount;
    lastIterationTickCount = currentTickCount;

    /* 
        If there has been a tick this means time has passed in a measurable way, we simulate that the
            hardware has consumed from the buffer as many times as the ticks have counted.
        If there has been no ticks since the last call all we have is the buffer, if there is space then
            great we accept the data, if not we accept what we have space left for, if any.
    */
    if (ticksSinceLastCall > 0)
    {
        /* Catch the buffer condition up since the last call to this function. This is done by subtracting number of ticks times rate 
            from the buffer, if this is less than 0 we set the buffer to empty.  */
        long maxConsumption = ticksSinceLastCall * RATE;
        if (maxConsumption >= bufferIndex) {
            bufferIndex = 0;
        } else {
            bufferIndex -= maxConsumption;
        }
    }

    // We allow at most the number of open spaces in the buffer to be sent
    long bytesAllowed = BUFLEN - bufferIndex;

    // Calculate what is the most data we can send, this is the min of butesAllowed and length offered
    long bytesToSend = bytesAllowed < length ? bytesAllowed : length;

    // Copy every byte into our hardware buffer here
    for (int i=0; i< bytesToSend; i++)
    {
        fakeSendRegister = ((uint8_t*)data)[i];
        bufferIndex++;
    }

    // If all the data fits in the buffer we send it all, else we send only the allowed amount.
    //   we indicate this by returning the number of bytes accepted into the buffer
    return bytesToSend;
}