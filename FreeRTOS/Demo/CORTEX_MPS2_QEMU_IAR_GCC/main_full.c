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
 *******************************************************************************
 * This project provides two demo applications.  A simple blinky style project,
 * and a more comprehensive test and demo application.  The
 * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting in main.c is used to select
 * between the two.  See the notes on using mainCREATE_SIMPLE_BLINKY_DEMO_ONLY
 * in main.c.  This file implements the comprehensive test and demo version.
 *
 * This file only contains the source code that is specific to the full demo.
 * Generic functions, such FreeRTOS hook functions, are defined in main.c.
 *******************************************************************************
 *
 * main() creates all the demo application tasks, then starts the scheduler.
 * The web documentation provides more details of the standard demo application
 * tasks, which provide no particular functionality but do provide a good
 * example of how to use the FreeRTOS API.
 *
 * In addition to the standard demo tasks, the following tasks and tests are
 * defined and/or created within this file:
 *
 * "Check" task - This only executes every five (simulated) seconds.  Its main
 * function is to check the tests running in the standard demo tasks have never
 * failed and that all the tasks are still running.  If that is the case the
 * check task prints "PASS : nnnn (x)", where nnnn is the current tick count and
 * x is the number of times the interrupt nesting test executed while interrupts
 * were nested.  If the check task discovers a failed test or a stalled task
 * it prints a message that indicates which task reported the error or stalled.
 * Normally the check task would have the highest priority to keep its timing
 * jitter to a minimum.  In this case the check task is run at the idle priority
 * to ensure other tasks are not stalled by it writing to a slow UART using a
 * polling driver.
 *
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Demo app includes. */
#include "death.h"
#include "blocktim.h"
#include "semtest.h"
#include "PollQ.h"
#include "GenQTest.h"
#include "QPeek.h"
#include "recmutex.h"
#include "IntQueue.h"
#include "QueueSet.h"
#include "EventGroupsDemo.h"
#include "MessageBufferDemo.h"
#include "StreamBufferDemo.h"
#include "AbortDelay.h"
#include "countsem.h"
#include "dynamic.h"
#include "MessageBufferAMP.h"
#include "QueueOverwrite.h"
#include "QueueSetPolling.h"
#include "StaticAllocation.h"
#include "TaskNotify.h"
#include "TaskNotifyArray.h"
#include "TimerDemo.h"
#include "StreamBufferInterrupt.h"
#include "IntSemTest.h"

/*-----------------------------------------------------------*/

/* Task priorities. */
#define mainQUEUE_POLL_PRIORITY          ( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY          ( tskIDLE_PRIORITY + 3 )
#define mainSEM_TEST_PRIORITY            ( tskIDLE_PRIORITY + 1 )
#define mainCREATOR_TASK_PRIORITY        ( tskIDLE_PRIORITY + 3 )
#define mainGEN_QUEUE_TASK_PRIORITY      ( tskIDLE_PRIORITY )

/* Stack sizes are defined relative to configMINIMAL_STACK_SIZE so they scale
 * across projects that have that constant set differently - in this case the
 * constant is different depending on the compiler in use. */
#define mainMESSAGE_BUFFER_STACK_SIZE    ( configMINIMAL_STACK_SIZE + ( configMINIMAL_STACK_SIZE >> 1 ) )
#define mainCHECK_TASK_STACK_SIZE        ( configMINIMAL_STACK_SIZE + ( configMINIMAL_STACK_SIZE >> 1 ) )

/* Parameters that are passed into the register check tasks solely for the
 * purpose of ensuring parameters are passed into tasks correctly. */
#define mainREG_TEST_TASK_1_PARAMETER    ( ( void * ) 0x12345678 )
#define mainREG_TEST_TASK_2_PARAMETER    ( ( void * ) 0x87654321 )

/*-----------------------------------------------------------*/

/*
 * Register check tasks, and the tasks used to write over and check the contents
 * of the FPU registers, as described at the top of this file. The nature of
 * these files necessitates that they are written in an assembly file, but the
 * entry points are kept in the C file for the convenience of checking the task
 * parameter.
 */
static void prvRegTestTaskEntry1( void * pvParameters );
extern void vRegTest1Implementation( void );
static void prvRegTestTaskEntry2( void * pvParameters );
extern void vRegTest2Implementation( void );

/* The task that checks the operation of all the other standard demo tasks, as
 * described at the top of this file. */
static void prvCheckTask( void * pvParameters );

/*-----------------------------------------------------------*/

/* The following two variables are used to communicate the status of the
 * register test tasks to the check task.  If the variables keep incrementing,
 * then the register test tasks have not discovered any errors.  If a variable
 * stops incrementing, then an error has been found. */
volatile unsigned long ulRegTest1LoopCounter = 0UL, ulRegTest2LoopCounter = 0UL;

/*-----------------------------------------------------------*/

void main_full( void )
{
    /* Start the standard demo tasks. */
    vStartGenericQueueTasks( mainGEN_QUEUE_TASK_PRIORITY );
    vStartInterruptQueueTasks();
    vStartRecursiveMutexTasks();
    vCreateBlockTimeTasks();
    vStartSemaphoreTasks( mainSEM_TEST_PRIORITY );
    vStartPolledQueueTasks( mainQUEUE_POLL_PRIORITY );
    vStartQueuePeekTasks();
    vStartQueueSetTasks();
    vStartEventGroupTasks();
    vStartMessageBufferTasks( mainMESSAGE_BUFFER_STACK_SIZE );
    vStartStreamBufferTasks();
    vCreateAbortDelayTasks();
    vStartCountingSemaphoreTasks();
    vStartDynamicPriorityTasks();
    vStartMessageBufferAMPTasks( configMINIMAL_STACK_SIZE );
    vStartQueueOverwriteTask( tskIDLE_PRIORITY );
    vStartQueueSetPollingTask();
    vStartStaticallyAllocatedTasks();
    vStartTaskNotifyTask();
    vStartTaskNotifyArrayTask();
    vStartTimerDemoTask( 50 );
    vStartStreamBufferInterruptDemo();
    vStartInterruptSemaphoreTasks();

    /* Create the register check tasks, as described at the top of this	file */
    xTaskCreate( prvRegTestTaskEntry1,          /* Function that implements the task. */
                 "Reg1",                        /* Human readable name for the task - not used by the kernel but helps debugging. */
                 configMINIMAL_STACK_SIZE,      /* Size of stack to allocate for the task - in words not bytes. */
                 mainREG_TEST_TASK_1_PARAMETER, /* A parameter passed into the task to check parameter passing is working correctly. */
                 tskIDLE_PRIORITY,              /* Priority assigned to the task - must be between 0 (tskIDLE_PRIORITY) and (configMAX_PRIORITIES - 1). */
                 NULL );                        /* Can be used to pass a handle to the create task out of the xTaskCreate() function. */
    xTaskCreate( prvRegTestTaskEntry2, "Reg2", configMINIMAL_STACK_SIZE, mainREG_TEST_TASK_2_PARAMETER, tskIDLE_PRIORITY, NULL );

    /* The suicide tasks must be created last as they need to know how many
     * tasks were running prior to their creation in order to ascertain whether
     * or not the correct/expected number of tasks are running at any given time. */
    vCreateSuicidalTasks( mainCREATOR_TASK_PRIORITY );

    xTaskCreate( prvCheckTask, "Check", mainCHECK_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If configSUPPORT_STATIC_ALLOCATION was false then execution would only
     * get here if there was insufficient heap memory to create either the idle or
     * timer tasks.  As static allocation is used execution should never be able
     * to reach here. */
    for( ; ; )
    {
    }
}
/*-----------------------------------------------------------*/

/* See the comments at the top of this file. */
static void prvCheckTask( void * pvParameters )
{
    static const char * pcMessage = "PASS";
    unsigned long ulLastRegTest1Value = 0, ulLastRegTest2Value = 0;

    const TickType_t xTaskPeriod = pdMS_TO_TICKS( 5000UL );
    TickType_t xPreviousWakeTime;
    extern uint32_t ulNestCount;

    /* Avoid warning about unused parameter. */
    ( void ) pvParameters;

    xPreviousWakeTime = xTaskGetTickCount();

    for( ; ; )
    {
        vTaskDelayUntil( &xPreviousWakeTime, xTaskPeriod );

        /* Has an error been found in any task? */
        if( xAreStreamBufferTasksStillRunning() != pdTRUE )
        {
            pcMessage = "xAreStreamBufferTasksStillRunning() returned false";
        }
        else if( xAreMessageBufferTasksStillRunning() != pdTRUE )
        {
            pcMessage = "xAreMessageBufferTasksStillRunning() returned false";
        }

        if( xAreGenericQueueTasksStillRunning() != pdTRUE )
        {
            pcMessage = "xAreGenericQueueTasksStillRunning() returned false";
        }
        else if( xIsCreateTaskStillRunning() != pdTRUE )
        {
            pcMessage = "xIsCreateTaskStillRunning() returned false";
        }
        else if( xAreIntQueueTasksStillRunning() != pdTRUE )
        {
            pcMessage = "xAreIntQueueTasksStillRunning() returned false";
        }
        else if( xAreBlockTimeTestTasksStillRunning() != pdTRUE )
        {
            pcMessage = "xAreBlockTimeTestTasksStillRunning() returned false";
        }
        else if( xAreSemaphoreTasksStillRunning() != pdTRUE )
        {
            pcMessage = "xAreSemaphoreTasksStillRunning() returned false";
        }
        else if( xArePollingQueuesStillRunning() != pdTRUE )
        {
            pcMessage = "xArePollingQueuesStillRunning() returned false";
        }
        else if( xAreQueuePeekTasksStillRunning() != pdTRUE )
        {
            pcMessage = "xAreQueuePeekTasksStillRunning() returned false";
        }
        else if( xAreRecursiveMutexTasksStillRunning() != pdTRUE )
        {
            pcMessage = "xAreRecursiveMutexTasksStillRunning() returned false";
        }
        else if( xAreQueueSetTasksStillRunning() != pdPASS )
        {
            pcMessage = "xAreQueueSetTasksStillRunning() returned false";
        }
        else if( xAreEventGroupTasksStillRunning() != pdTRUE )
        {
            pcMessage = "xAreEventGroupTasksStillRunning() returned false";
        }
        else if( xAreAbortDelayTestTasksStillRunning() != pdTRUE )
        {
            pcMessage = "xAreAbortDelayTestTasksStillRunning() returned false";
        }
        else if( xAreCountingSemaphoreTasksStillRunning() != pdTRUE )
        {
            pcMessage = "xAreCountingSemaphoreTasksStillRunning() returned false";
        }
        else if( xAreDynamicPriorityTasksStillRunning() != pdTRUE )
        {
            pcMessage = "xAreDynamicPriorityTasksStillRunning() returned false";
        }
        else if( xAreMessageBufferAMPTasksStillRunning() != pdTRUE )
        {
            pcMessage = "xAreMessageBufferAMPTasksStillRunning() returned false";
        }
        else if( xIsQueueOverwriteTaskStillRunning() != pdTRUE )
        {
            pcMessage = "xIsQueueOverwriteTaskStillRunning() returned false";
        }
        else if( xAreQueueSetPollTasksStillRunning() != pdTRUE )
        {
            pcMessage = "xAreQueueSetPollTasksStillRunning() returned false";
        }
        else if( xAreStaticAllocationTasksStillRunning() != pdTRUE )
        {
            pcMessage = "xAreStaticAllocationTasksStillRunning() returned false";
        }
        else if( xAreTaskNotificationTasksStillRunning() != pdTRUE )
        {
            pcMessage = "xAreTaskNotificationTasksStillRunning() returned false";
        }
        else if( xAreTaskNotificationArrayTasksStillRunning() != pdTRUE )
        {
            pcMessage = "xAreTaskNotificationArrayTasksStillRunning() returned false";
        }
        else if( xAreTimerDemoTasksStillRunning( xTaskPeriod ) != pdTRUE )
        {
            pcMessage = "xAreTimerDemoTasksStillRunning() returned false";
        }
        else if( xIsInterruptStreamBufferDemoStillRunning() != pdTRUE )
        {
            pcMessage = "xIsInterruptStreamBufferDemoStillRunning() returned false";
        }
        else if( xAreInterruptSemaphoreTasksStillRunning() != pdTRUE )
        {
            pcMessage = "xAreInterruptSemaphoreTasksStillRunning() returned false";
        }
        else if( ulLastRegTest1Value == ulRegTest1LoopCounter )
        {
            /* Check that the register test 1 task is still running. */
            pcMessage = "Error in RegTest 1";
        }
        else if( ulLastRegTest2Value == ulRegTest2LoopCounter )
        {
            /* Check that the register test 2 task is still running. */
            pcMessage = "Error in RegTest 2";
        }

        ulLastRegTest1Value = ulRegTest1LoopCounter;
        ulLastRegTest2Value = ulRegTest2LoopCounter;

        /* It is normally not good to call printf() from an embedded system,
         * although it is ok in this simulated case. */
        printf( "%s : %d (%d)\r\n", pcMessage, ( int ) xTaskGetTickCount(), ( int ) ulNestCount );
    }
}
/*-----------------------------------------------------------*/

void vFullDemoTickHookFunction( void )
{
    /* Write to a queue that is in use as part of the queue set demo to
     * demonstrate using queue sets from an ISR. */
    vQueueSetAccessQueueSetFromISR();

    /* Call the event group ISR tests. */
    vPeriodicEventGroupsProcessing();

    /* Exercise stream buffers from interrupts. */
    vPeriodicStreamBufferProcessing();

    /* Exercise using queue overwrites from interrupts. */
    vQueueOverwritePeriodicISRDemo();

    /* Exercise using Queue Sets from interrupts. */
    vQueueSetPollingInterruptAccess();

    /* Exercise using task notifications from interrupts. */
    xNotifyTaskFromISR();
    xNotifyArrayTaskFromISR();

    /* Exercise software timers from interrupts. */
    vTimerPeriodicISRTests();

    /* Exercise stream buffers from interrupts. */
    vBasicStreamBufferSendFromISR();

    /* Exercise semaphores from interrupts. */
    vInterruptSemaphorePeriodicTest();
}
/*-----------------------------------------------------------*/

static void prvRegTestTaskEntry1( void * pvParameters )
{
    /* Although the regtest task is written in assembler, its entry point is
     * written in C for convenience of checking the task parameter is being passed
     * in correctly. */
    if( pvParameters == mainREG_TEST_TASK_1_PARAMETER )
    {
        /* Start the part of the test that is written in assembler. */
        vRegTest1Implementation();
    }

    /* The following line will only execute if the task parameter is found to
     * be incorrect. The check task will detect that the regtest loop counter is
     * not being incremented and flag an error. */
    vTaskDelete( NULL );
}
/*-----------------------------------------------------------*/

static void prvRegTestTaskEntry2( void * pvParameters )
{
    /* Although the regtest task is written in assembler, its entry point is
     * written in C for convenience of checking the task parameter is being passed
     * in correctly. */
    if( pvParameters == mainREG_TEST_TASK_2_PARAMETER )
    {
        /* Start the part of the test that is written in assembler. */
        vRegTest2Implementation();
    }

    /* The following line will only execute if the task parameter is found to
     * be incorrect. The check task will detect that the regtest loop counter is
     * not being incremented and flag an error. */
    vTaskDelete( NULL );
}
/*-----------------------------------------------------------*/

