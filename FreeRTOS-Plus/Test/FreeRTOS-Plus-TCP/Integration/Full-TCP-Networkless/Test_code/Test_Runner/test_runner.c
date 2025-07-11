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

/**
 * @file aws_test_runner.c
 * @brief The function to be called to run all the tests.
 */

/* Test runner interface includes. */
#include "test_runner.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Unity framework includes. */
#include "unity_fixture.h"
#include "unity_internals.h"

char cBuffer[ testrunnerBUFFER_SIZE ];

/* Heap leak variables. */
unsigned int xHeapBefore;
unsigned int xHeapAfter;
/*-----------------------------------------------------------*/

/* This function will be generated by the test automation framework,
 * do not change the signature of it. You could, however, add or remove
 * RUN_TEST_GROUP statements.
 */
static void RunTests( void )
{
    RUN_TEST_GROUP( Full_FREERTOS_TCP );
}
/*-----------------------------------------------------------*/

void TEST_RUNNER_RunTests_task( void * pvParameters )
{
    /* Disable unused parameter warning. */
    ( void ) pvParameters;

    /* Initialize unity. */
    UnityFixture.Verbose = 1;
    UnityFixture.GroupFilter = 0;
    UnityFixture.NameFilter = testrunnerTEST_FILTER;
    UnityFixture.RepeatCount = 1;

    UNITY_BEGIN();

    /* Give the print buffer time to empty */
    vTaskDelay( pdMS_TO_TICKS( 500 ) );
    /* Measure the heap size before any tests are run. */
    #if ( testrunnerFULL_MEMORYLEAK_ENABLED == 1 )
        xHeapBefore = xPortGetFreeHeapSize();
    #endif

    RunTests();

    #if ( testrunnerFULL_MEMORYLEAK_ENABLED == 1 )

        /* Measure the heap size after tests are done running.
         * This test must run last. */

        /* Perform any global resource cleanup necessary to avoid memory leaks. */
        #ifdef testrunnerMEMORYLEAK_CLEANUP
            testrunnerMEMORYLEAK_CLEANUP();
        #endif

        /* Give the print buffer time to empty */
        vTaskDelay( pdMS_TO_TICKS( 500 ) );
        xHeapAfter = xPortGetFreeHeapSize();
        RUN_TEST_GROUP( Full_MemoryLeak );
    #endif /* if ( testrunnerFULL_MEMORYLEAK_ENABLED == 1 ) */

    /* Currently disabled. Will be enabled after cleanup. */
    UNITY_END();

    #ifdef CODE_COVERAGE
        exit( 0 );
    #endif

    /* This task has finished.  FreeRTOS does not allow a task to run off the
     * end of its implementing function, so the task must be deleted. */
    vTaskDelete( NULL );
}
/*-----------------------------------------------------------*/
