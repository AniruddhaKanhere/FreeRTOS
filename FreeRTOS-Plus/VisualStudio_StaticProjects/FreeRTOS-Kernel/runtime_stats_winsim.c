/*
 * FreeRTOS V12345
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/*
 * Utility functions required to gather run time statistics.  See:
 * https://www.FreeRTOS.org/rtos-run-time-stats.html
 *
 * Note that this is a simulated port, where simulated time is a lot slower than
 * real time, therefore the run time counter values have no real meaningful
 * units.
 *
 * Also note that it is assumed this demo is going to be used for short periods
 * of time only, and therefore timer overflows are not handled.
 */

#include <time.h>

#ifdef WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
#else
    #include <winsock.h>
#endif /* WIN32_LEAN_AND_MEAN */

#include <windows.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

#if configGENERATE_RUN_TIME_STATS == 1

/* Time at start of day (in ns). */
    static LARGE_INTEGER lStartTime;

/*-----------------------------------------------------------*/

    void vConfigureTimerForRunTimeStats( void )
    {
        ( void ) QueryPerformanceCounter( &lStartTime );
    }
/*-----------------------------------------------------------*/

    unsigned long ulGetRunTimeCounterValue( void )
    {
        LARGE_INTEGER lCurrentTime;

        ( void ) QueryPerformanceCounter( &lCurrentTime );

        configASSERT( lCurrentTime.QuadPart > lStartTime.QuadPart );

        return ( unsigned long ) ( lCurrentTime.QuadPart - lStartTime.QuadPart );
    }
/*-----------------------------------------------------------*/
#endif /* configGENERATE_RUN_TIME_STATS == 1 */
