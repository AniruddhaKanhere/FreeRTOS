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

/***
 * See https://www.FreeRTOS.org/coremqtt for configuration and usage instructions.
 ***/

/* Standard includes. */
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo logging includes. */
#include "logging.h"

/* Demo Specific configs. */
#include "demo_config.h"

/*-----------------------------------------------------------*/

/* FreeRTOS Cellular Library init and setup cellular network registration. */
extern bool setupCellular( void );

/* The MQTT demo entry function. */
extern void vStartSimpleMQTTDemo( void );

/* Task to handle connecting the cellular module. */
static void vCellularDemoTask( void * pvParameters );

/*-----------------------------------------------------------*/


static void vCellularDemoTask( void * pvParameters )
{
    ( void ) pvParameters;

    /* Setup cellular. */
    configASSERT( setupCellular() == true );

    vStartSimpleMQTTDemo();

    vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/

int main( void )
{
    vPlatformInitLogging();

    /* FreeRTOS Cellular Library init needs thread ready environment.
     * CellularDemoTask invoke setupCellular to init FreeRTOS Cellular Library and register network.
     * Then it runs the MQTT demo. */
    xTaskCreate( vCellularDemoTask,        /* Function that implements the task. */
                 "CellularConnect",        /* Text name for the task - only used for debugging. */
                 democonfigDEMO_STACKSIZE, /* Size of stack (in words, not bytes) to allocate for the task. */
                 NULL,                     /* Task parameter - not used in this case. */
                 democonfigDEMO_PRIORITY,  /* Task priority, must be between 0 and configMAX_PRIORITIES - 1. */
                 NULL );                   /* Used to pass out a handle to the created task - not used in this case. */

    /* Start the RTOS scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following
     * line will never be reached.  If the following line does execute, then
     * there was insufficient FreeRTOS heap memory available for the idle and/or
     * timer tasks to be created.  See the memory management section on the
     * FreeRTOS web site for more details.
     */

    for( ; ; )
    {
        configASSERT( pdFALSE );
    }
}
/*-----------------------------------------------------------*/
