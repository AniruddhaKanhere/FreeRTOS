/*
 * FreeRTOS V12345
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

    .extern ulRegTest1LoopCounter
    .extern ulRegTest2LoopCounter

    .global vRegTest1Implementation
    .global vRegTest2Implementation

/*-----------------------------------------------------------*/

/* Constants to define the additional registers found on the Pulpino RI5KY. */
#define lpstart0     0x7b0
#define lpend0       0x7b1
#define lpcount0     0x7b2
#define lpstart1     0x7b4
#define lpend1       0x7b5
#define lpcount1     0x7b6

/*-----------------------------------------------------------*/

/*
 * The register check tasks are described in the comments at the top of
 * main_full.c.
 */

.align( 4 )
vRegTest1Implementation:

    /* Fill the additional registers with known values. */
    li t0, 0xaa
    csrw lpstart0, t0
    li t0, 0xab
    csrw lpstart1, t0
    li t0, 0xac
    csrw lpend0, t0
    li t0, 0xad
    csrw lpend1, t0
    li t0, 0xae
    csrw lpcount0, t0
    li t0, 0xaf
    csrw lpcount1, t0

    /* Fill the core registers with known values. */
    li x5, 0x5
    li x6, 0x6
    li x7, 0x7
    li x8, 0x8
    li x9, 0x9
    li x10, 0xa
    li x11, 0xb
    li x12, 0xc
    li x13, 0xd
    li x14, 0xe
    li x15, 0xf
    li x16, 0x10
    li x17, 0x11
    li x18, 0x12
    li x19, 0x13
    li x20, 0x14
    li x21, 0x15
    li x22, 0x16
    li x23, 0x17
    li x24, 0x18
    li x25, 0x19
    li x26, 0x1a
    li x27, 0x1b
    li x28, 0x1c
    li x29, 0x1d
    li x30, 0x1e

reg1_loop:

    /* Check each register still contains the expected known value.
    vRegTest1Implementation uses x31 as the temporary, vRegTest2Implementation
    uses x5 as the temporary. */
    li x31, 0x5
    bne x31, x5, reg1_error_loop
    li x31, 0x6
    bne x31, x6, reg1_error_loop
    li x31, 0x7
    bne x31, x7, reg1_error_loop
    li x31, 0x8
    bne x31, x8, reg1_error_loop
    li x31, 0x9
    bne x31, x9, reg1_error_loop
    li x31, 0xa
    bne x31, x10, reg1_error_loop
    li x31, 0xb
    bne x31, x11, reg1_error_loop
    li x31, 0xc
    bne x31, x12, reg1_error_loop
    li x31, 0xd
    bne x31, x13, reg1_error_loop
    li x31, 0xe
    bne x31, x14, reg1_error_loop
    li x31, 0xf
    bne x31, x15, reg1_error_loop
    li x31, 0x10
    bne x31, x16, reg1_error_loop
    li x31, 0x11
    bne x31, x17, reg1_error_loop
    li x31, 0x12
    bne x31, x18, reg1_error_loop
    li x31, 0x13
    bne x31, x19, reg1_error_loop
    li x31, 0x14
    bne x31, x20, reg1_error_loop
    li x31, 0x15
    bne x31, x21, reg1_error_loop
    li x31, 0x16
    bne x31, x22, reg1_error_loop
    li x31, 0x17
    bne x31, x23, reg1_error_loop
    li x31, 0x18
    bne x31, x24, reg1_error_loop
    li x31, 0x19
    bne x31, x25, reg1_error_loop
    li x31, 0x1a
    bne x31, x26, reg1_error_loop
    li x31, 0x1b
    bne x31, x27, reg1_error_loop
    li x31, 0x1c
    bne x31, x28, reg1_error_loop
    li x31, 0x1d
    bne x31, x29, reg1_error_loop
    li x31, 0x1e
    bne x31, x30, reg1_error_loop

    /* Check additional chip specific registers still contain the expected
    values. */
    csrr x30, lpstart0
    li x31, 0xaa
    bne x30, x31, reg1_error_loop
    csrr x30, lpstart1
    li x31, 0xab
    bne x30, x31, reg1_error_loop
    csrr x30, lpend0
    li x31, 0xac
    bne x30, x31, reg1_error_loop
    csrr x30, lpend1
    li x31, 0xad
    bne x30, x31, reg1_error_loop
    csrr x30, lpcount0
    li x31, 0xae
    bne x30, x31, reg1_error_loop
    csrr x30, lpcount1
    li x31, 0xaf
    bne x30, x31, reg1_error_loop

    /* Everything passed, increment the loop counter. */
    lw x31, ulRegTest1LoopCounterConst
    lw x30, 0(x31)
    addi x30, x30, 1
    sw x30, 0(x31)

    /* Restore clobbered register reading for next loop. */
    li x30, 0x1e

    /* Yield to increase code coverage. */
    ecall

    /* Start again. */
    jal reg1_loop

reg1_error_loop:
    /* Busy loop which holds the task. */
    jal reg1_error_loop

.align( 4 )
ulRegTest1LoopCounterConst: .word ulRegTest1LoopCounter

/*-----------------------------------------------------------*/

.align( 4 )
vRegTest2Implementation:

    /* Fill the additional registers with known values. */
    li t0, 0x1aa
    csrw lpstart0, t0
    li t0, 0x1ab
    csrw lpstart1, t0
    li t0, 0x1ac
    csrw lpend0, t0
    li t0, 0x1ad
    csrw lpend1, t0
    li t0, 0x1ae
    csrw lpcount0, t0
    li t0, 0x1af
    csrw lpcount1, t0

    /* Fill the core registers with known values. */
    li x6, 0x61
    li x7, 0x71
    li x8, 0x81
    li x9, 0x91
    li x10, 0xa1
    li x11, 0xb1
    li x12, 0xc1
    li x13, 0xd1
    li x14, 0xe1
    li x15, 0xf1
    li x16, 0x20
    li x17, 0x21
    li x18, 0x22
    li x19, 0x23
    li x20, 0x24
    li x21, 0x25
    li x22, 0x26
    li x23, 0x27
    li x24, 0x28
    li x25, 0x29
    li x26, 0x2a
    li x27, 0x2b
    li x28, 0x2c
    li x29, 0x2d
    li x30, 0x2e
    li x31, 0x2f

Reg2_loop:

    /* Check each register still contains the expected known value.
    vRegTest2Implementation uses x5 as the temporary, vRegTest1Implementation
    uses x31 as the temporary. */
    li x5, 0x61
    bne x5, x6, reg2_error_loop
    li x5, 0x71
    bne x5, x7, reg2_error_loop
    li x5, 0x81
    bne x5, x8, reg2_error_loop
    li x5, 0x91
    bne x5, x9, reg2_error_loop
    li x5, 0xa1
    bne x5, x10, reg2_error_loop
    li x5, 0xb1
    bne x5, x11, reg2_error_loop
    li x5, 0xc1
    bne x5, x12, reg2_error_loop
    li x5, 0xd1
    bne x5, x13, reg2_error_loop
    li x5, 0xe1
    bne x5, x14, reg2_error_loop
    li x5, 0xf1
    bne x5, x15, reg2_error_loop
    li x5, 0x20
    bne x5, x16, reg2_error_loop
    li x5, 0x21
    bne x5, x17, reg2_error_loop
    li x5, 0x22
    bne x5, x18, reg2_error_loop
    li x5, 0x23
    bne x5, x19, reg2_error_loop
    li x5, 0x24
    bne x5, x20, reg2_error_loop
    li x5, 0x25
    bne x5, x21, reg2_error_loop
    li x5, 0x26
    bne x5, x22, reg2_error_loop
    li x5, 0x27
    bne x5, x23, reg2_error_loop
    li x5, 0x28
    bne x5, x24, reg2_error_loop
    li x5, 0x29
    bne x5, x25, reg2_error_loop
    li x5, 0x2a
    bne x5, x26, reg2_error_loop
    li x5, 0x2b
    bne x5, x27, reg2_error_loop
    li x5, 0x2c
    bne x5, x28, reg2_error_loop
    li x5, 0x2d
    bne x5, x29, reg2_error_loop
    li x5, 0x2e
    bne x5, x30, reg2_error_loop
    li x5, 0x2f
    bne x5, x31, reg2_error_loop

    /* Check additional chip specific registers still contain the expected
    values. */
    csrr x5, lpstart0
    li x6, 0x1aa
    bne x5, x6, reg2_error_loop
    csrr x5, lpstart1
    li x6, 0x1ab
    bne x5, x6, reg2_error_loop
    csrr x5, lpend0
    li x6, 0x1ac
    bne x5, x6, reg2_error_loop
    csrr x5, lpend1
    li x6, 0x1ad
    bne x5, x6, reg2_error_loop
    csrr x5, lpcount0
    li x6, 0x1ae
    bne x5, x6, reg2_error_loop
    csrr x5, lpcount1
    li x6, 0x1af
    bne x5, x6, reg2_error_loop

    /* Everything passed, increment the loop counter. */
    lw x5, ulRegTest2LoopCounterConst
    lw x6, 0(x5)
    addi x6, x6, 1
    sw x6, 0(x5)

    /* Restore clobbered register reading for next loop. */
    li x6, 0x61

    /* Start again. */
    jal Reg2_loop

reg2_error_loop:
    /* Busy loop which holds the task. */
    jal reg2_error_loop

.align( 4 )
ulRegTest2LoopCounterConst: .word ulRegTest2LoopCounter
