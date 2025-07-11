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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void vPortSVCHandler( void );
extern void xPortPendSVHandler( void );
extern void xPortSysTickHandler( void );
extern void uart_init( void );
extern int main( void );

void _start( void );
void EthernetISR( void );
void Reset_Handler( void );

extern uint32_t _estack, _sidata, _sdata, _edata, _sbss, _ebss;

/* Prevent optimization so gcc does not replace code with memcpy */
__attribute__( ( optimize( "O0" ) ) )
__attribute__( ( naked ) )
void Reset_Handler( void )
{
    /* set stack pointer */
    __asm volatile ( "ldr r0, =_estack" );
    __asm volatile ( "mov sp, r0" );

    /* copy .data section from flash to RAM */
    for( uint32_t * src = &_sidata, * dest = &_sdata; dest < &_edata; )
    {
        *dest++ = *src++;
    }

    /* zero out .bss section */
    for( uint32_t * dest = &_sbss; dest < &_ebss; )
    {
        *dest++ = 0;
    }

    /* jump to board initialisation */
    _start();
}

__attribute__( ( used ) ) static void prvGetRegistersFromStack( uint32_t * pulFaultStackAddress )
{
/* These are volatile to try and prevent the compiler/linker optimizing them
 * away as the variables never actually get used.  If the debugger won't show the
 * values of the variables, make them global my moving their declaration outside
 * of this function. */
    volatile uint32_t r0;
    volatile uint32_t r1;
    volatile uint32_t r2;
    volatile uint32_t r3;
    volatile uint32_t r12;
    volatile uint32_t lr;  /* Link register. */
    volatile uint32_t pc;  /* Program counter. */
    volatile uint32_t psr; /* Program status register. */

    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];

    /* When the following line is hit, the variables contain the register values. */
    for( ; ; )
    {
    }

    /* Remove the warning about unused variables. */
    ( void ) r0;
    ( void ) r1;
    ( void ) r2;
    ( void ) r3;
    ( void ) r12;
    ( void ) lr;
    ( void ) pc;
    ( void ) psr;
}

static void Default_Handler( void ) __attribute__( ( naked ) );
void Default_Handler( void )
{
    __asm volatile
    (
        "Default_Handler:\n"
        "    ldr r3, =0xe000ed04\n"
        "    ldr r2, [r3, #0]\n"
        "    uxtb r2, r2\n"
        "Infinite_Loop:\n"
        "    b  Infinite_Loop\n"
        ".size  Default_Handler, .-Default_Handler\n"
        ".ltorg\n"
    );
}

static void Default_Handler2( void ) __attribute__( ( naked ) );
void Default_Handler2( void )
{
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, =prvGetRegistersFromStack                         \n"
        " bx r2                                                     \n"
        " .ltorg                                                    \n"
    );
}

static void Default_Handler3( void )
{
    for( ; ; )
    {
    }
}

static void Default_Handler4( void )
{
    for( ; ; )
    {
    }
}

static void Default_Handler5( void )
{
    for( ; ; )
    {
    }
}

static void Default_Handler6( void )
{
    for( ; ; )
    {
    }
}

const uint32_t * const isr_vector[] __attribute__( ( section( ".isr_vector" ), used ) ) =
{
    ( uint32_t * ) &_estack,
    ( uint32_t * ) &Reset_Handler,       /* Reset                -15  */
    ( uint32_t * ) &Default_Handler,     /* NMI_Handler          -14  */
    ( uint32_t * ) &Default_Handler2,    /* HardFault_Handler    -13  */
    ( uint32_t * ) &Default_Handler3,    /* MemManage_Handler    -12  */
    ( uint32_t * ) &Default_Handler4,    /* BusFault_Handler     -11  */
    ( uint32_t * ) &Default_Handler5,    /* UsageFault_Handler   -10  */
    0,                                   /* reserved                  */
    0,                                   /* reserved                  */
    0,                                   /* reserved                  */
    0,                                   /* reserved             -6   */
    ( uint32_t * ) &vPortSVCHandler,     /* SVC_Handler          -5   */
    ( uint32_t * ) &Default_Handler6,    /* DebugMon_Handler     -4   */
    0,                                   /* reserved                  */
    ( uint32_t * ) &xPortPendSVHandler,  /* PendSV handler       -2   */
    ( uint32_t * ) &xPortSysTickHandler, /* SysTick_Handler      -1   */
    0,                                   /* uart0 receive         0   */
    0,                                   /* uart0 transmit            */
    0,                                   /* uart1 receive             */
    0,                                   /* uart1 transmit            */
    0,                                   /* uart 2 receive            */
    0,                                   /* uart 2 transmit           */
    0,                                   /* GPIO 0 combined interrupt */
    0,                                   /* GPIO 2 combined interrupt */
    0,                                   /* Timer 0                   */
    0,                                   /* Timer 1                   */
    0,                                   /* Dial Timer                */
    0,                                   /* SPI0 SPI1                 */
    0,                                   /* uart overflow 1 2,3   12  */
    ( uint32_t * ) &EthernetISR,         /* Ethernet              13  */
};

void _start( void )
{
    uart_init();
    main();
    exit( 0 );
}

__attribute__( ( naked ) ) void exit( int status )
{
    /* Force qemu to exit using ARM Semihosting */
    __asm volatile (
        "mov r1, r0\n"
        "cmp r1, #0\n"
        "bne .notclean\n"
        "ldr r1, =0x20026\n" /* ADP_Stopped_ApplicationExit, a clean exit */
        ".notclean:\n"
        "movs r0, #0x18\n"   /* SYS_EXIT */
        "bkpt 0xab\n"
        "end: b end\n"
        ".ltorg"
        );

    ( void ) status;
}
