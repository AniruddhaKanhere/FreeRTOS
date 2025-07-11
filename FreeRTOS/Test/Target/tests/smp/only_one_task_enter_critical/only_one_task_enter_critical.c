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
 * @file only_one_task_enter_critical.c
 * @brief Only one task/ISR shall be able to enter critical section at a time.
 *
 * Procedure:
 *   - Create ( num of cores ) tasks.
 *   - All tasks increase the counter for TASK_INCREASE_COUNTER_TIMES times in
 *     critical section.
 * Expected:
 *   - All tasks have correct value of counter after increasing.
 */

/* Standard includes. */
#include <stdint.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Unit testing support functions. */
#include "unity.h"
/*-----------------------------------------------------------*/

/**
 * @brief As time of loop for task to increase counter.
 */
#define TASK_INCREASE_COUNTER_TIMES    ( 1000000 )

/**
 * @brief Timeout value to stop test.
 */
#define TEST_TIMEOUT_MS                ( 1000 )
/*-----------------------------------------------------------*/

/**
 * @brief Test case "Only One Task Enter Critical".
 */
void Test_OnlyOneTaskEnterCritical( void );

/**
 * @brief Task function to increase counter then keep delaying.
 */
static void prvTaskIncCounter( void * pvParameters );
/*-----------------------------------------------------------*/

#if ( configNUMBER_OF_CORES < 2 )
    #error This test is for FreeRTOS SMP and therefore, requires at least 2 cores.
#endif /* if ( configNUMBER_OF_CORES < 2 ) */

#if ( configMAX_PRIORITIES <= 2 )
    #error configMAX_PRIORITIES must be larger than 2 to avoid scheduling idle tasks unexpectedly.
#endif /* if ( configMAX_PRIORITIES <= 2 ) */
/*-----------------------------------------------------------*/

/**
 * @brief Handles of the tasks created in this test.
 */
static TaskHandle_t xTaskHandles[ configNUMBER_OF_CORES ];

/**
 * @brief Indexes of the tasks created in this test.
 */
static uint32_t xTaskIndexes[ configNUMBER_OF_CORES ];

/**
 * @brief Flags to indicate if task T0 ~ T(n - 1) finish or not.
 */
static BaseType_t xTaskTestResults[ configNUMBER_OF_CORES ];

/**
 * @brief Variables to indicate task is ready for testing.
 */
static volatile BaseType_t xTaskReady[ configNUMBER_OF_CORES ];

/**
 * @brief Counter for all tasks to increase.
 */
static volatile uint32_t xTaskCounter;
/*-----------------------------------------------------------*/

static void prvTaskIncCounter( void * pvParameters )
{
    uint32_t currentTaskIdx = *( ( uint32_t * ) pvParameters );
    BaseType_t xAllTaskReady = pdFALSE;
    BaseType_t xTestResult = pdPASS;
    uint32_t xTempTaskCounter = 0;
    uint32_t i;

    /* Ensure all test tasks are running in the task function. */
    xTaskReady[ currentTaskIdx ] = pdTRUE;

    while( xAllTaskReady == pdFALSE )
    {
        xAllTaskReady = pdTRUE;

        for( i = 0; i < configNUMBER_OF_CORES; i++ )
        {
            if( xTaskReady[ i ] != pdTRUE )
            {
                xAllTaskReady = pdFALSE;
                break;
            }
        }
    }

    /* Increase the test counter in the loop. The test expects only one task
     * can increase the shared variable xTaskCounter protected by the critical
     * section at the same time. */
    taskENTER_CRITICAL();
    {
        xTempTaskCounter = xTaskCounter;

        for( i = 0; i < TASK_INCREASE_COUNTER_TIMES; i++ )
        {
            /* Increase the local variable xTempTaskCounter and shared variable
             * xTaskCounter. They must have the same value in the critical
             * section. */
            xTaskCounter++;
            xTempTaskCounter++;

            /* If multiple tasks enter the critical section, the shared variable
             * xTaskCounter will be increased by multiple tasks.As a result, the
             * local variable xTempTaskCounter won't be equal to the shared
             * variable xTaskCounter. */
            if( xTaskCounter != xTempTaskCounter )
            {
                xTestResult = pdFAIL;
                break;
            }
        }
    }
    taskEXIT_CRITICAL();

    xTaskTestResults[ currentTaskIdx ] = xTestResult;

    /* Blocking the test task. */
    vTaskDelay( portMAX_DELAY );
}
/*-----------------------------------------------------------*/

void Test_OnlyOneTaskEnterCritical( void )
{
    uint32_t i;
    BaseType_t xTaskCreationResult;

    /* Create configNUMBER_OF_CORES low priority tasks. */
    for( i = 0; i < configNUMBER_OF_CORES; i++ )
    {
        xTaskCreationResult = xTaskCreate( prvTaskIncCounter,
                                           "IncCounter",
                                           configMINIMAL_STACK_SIZE,
                                           &( xTaskIndexes[ i ] ),
                                           configMAX_PRIORITIES - 2,
                                           &( xTaskHandles[ i ] ) );

        TEST_ASSERT_EQUAL_MESSAGE( pdPASS, xTaskCreationResult, "Task creation failed." );
    }

    /* Delay for other cores to run tasks. */
    vTaskDelay( pdMS_TO_TICKS( TEST_TIMEOUT_MS ) );

    /* Verify each test task result. */
    for( i = 0; i < configNUMBER_OF_CORES; i++ )
    {
        TEST_ASSERT_EQUAL_MESSAGE( pdPASS, xTaskTestResults[ i ], "Critical section test task failed." );
    }

    /* Verify the shared variable counter value. */
    TEST_ASSERT_EQUAL_UINT32( configNUMBER_OF_CORES * TASK_INCREASE_COUNTER_TIMES, xTaskCounter );
}
/*-----------------------------------------------------------*/

/* Runs before every test, put init calls here. */
void setUp( void )
{
    uint32_t i;

    xTaskCounter = 0;

    for( i = 0; i < configNUMBER_OF_CORES; i++ )
    {
        xTaskIndexes[ i ] = i;
        xTaskHandles[ i ] = NULL;
        xTaskTestResults[ i ] = pdFALSE;
        xTaskReady[ i ] = pdFALSE;
    }
}
/*-----------------------------------------------------------*/

/* Runs after every test, put clean-up calls here. */
void tearDown( void )
{
    uint32_t i;

    /* Delete all the tasks. */
    for( i = 0; i < configNUMBER_OF_CORES; i++ )
    {
        if( xTaskHandles[ i ] != NULL )
        {
            vTaskDelete( xTaskHandles[ i ] );
            xTaskHandles[ i ] = NULL;
        }
    }
}
/*-----------------------------------------------------------*/

/**
 * @brief Entry point for test runner to run critical section test.
 */
void vRunOnlyOneTaskEnterCriticalTest( void )
{
    UNITY_BEGIN();

    RUN_TEST( Test_OnlyOneTaskEnterCritical );

    UNITY_END();
}
/*-----------------------------------------------------------*/
