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

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "list.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DNS.h"

/* Test includes. */
#include "unity_fixture.h"
#include "unity.h"
#include "freertos_tcp_test_access_declare.h"

/**
 * @brief Configuration for this test group.
 */

/*
 * @brief Test group definition.
 */
TEST_GROUP( Full_FREERTOS_TCP );

TEST_SETUP( Full_FREERTOS_TCP )
{
}

TEST_TEAR_DOWN( Full_FREERTOS_TCP )
{
}

TEST_GROUP_RUNNER( Full_FREERTOS_TCP )
{
    /* Run a parser test. */
    RUN_TEST_CASE( Full_FREERTOS_TCP, prvParseDnsResponse );
    RUN_TEST_CASE( Full_FREERTOS_TCP, ulDNSHandlePacket );

    /* prvCheckOptions test. */
    RUN_TEST_CASE( Full_FREERTOS_TCP, prvCheckOptions );

    /* xProcessReceivedUDPPacket test. */
    RUN_TEST_CASE( Full_FREERTOS_TCP, UDPPacketLength );
}

TEST( Full_FREERTOS_TCP, prvParseDnsResponse )
{
    uint8_t ucGoodDnsResponse[] =
    {
        0xd7, 0x66, 0x81, 0x80, 0x00, 0x01, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x61, 0x33, 0x37,
        0x62, 0x78, 0x76, 0x31, 0x63, 0x62, 0x64, 0x61, 0x33, 0x6a, 0x67, 0x03, 0x69, 0x6f, 0x74, 0x09,
        0x75, 0x73, 0x2d, 0x77, 0x65, 0x73, 0x74, 0x2d, 0x32, 0x09, 0x61, 0x6d, 0x61, 0x7a, 0x6f, 0x6e,
        0x61, 0x77, 0x73, 0x03, 0x63, 0x6f, 0x6d, 0x00, 0x00, 0x01, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x05,
        0x00, 0x01, 0x00, 0x00, 0x01, 0x2c, 0x00, 0x1e, 0x0c, 0x69, 0x6f, 0x74, 0x6d, 0x6f, 0x6f, 0x6e,
        0x72, 0x61, 0x6b, 0x65, 0x72, 0x09, 0x75, 0x73, 0x2d, 0x77, 0x65, 0x73, 0x74, 0x2d, 0x32, 0x04,
        0x70, 0x72, 0x6f, 0x64, 0xc0, 0x1b, 0xc0, 0x48, 0x00, 0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0xec,
        0x00, 0x45, 0x09, 0x64, 0x75, 0x61, 0x6c, 0x73, 0x74, 0x61, 0x63, 0x6b, 0x2a, 0x69, 0x6f, 0x74,
        0x6d, 0x6f, 0x6f, 0x6e, 0x72, 0x61, 0x6b, 0x65, 0x72, 0x2d, 0x75, 0x2d, 0x65, 0x6c, 0x62, 0x2d,
        0x31, 0x77, 0x38, 0x71, 0x6e, 0x77, 0x31, 0x33, 0x33, 0x36, 0x7a, 0x71, 0x2d, 0x31, 0x31, 0x38,
        0x36, 0x33, 0x34, 0x38, 0x30, 0x39, 0x32, 0x09, 0x75, 0x73, 0x2d, 0x77, 0x65, 0x73, 0x74, 0x2d,
        0x32, 0x03, 0x65, 0x6c, 0x62, 0xc0, 0x29, 0xc0, 0x72, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x23, 0x00, 0x04, 0x22, 0xd3, 0x41, 0xdb, 0xc0, 0x72, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x23, 0x00, 0x04, 0x22, 0xd3, 0x53, 0xe4, 0xc0, 0x72, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x23, 0x00, 0x04, 0x22, 0xd3, 0xb6, 0x17, 0xc0, 0x72, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x23, 0x00, 0x04, 0x22, 0xd6, 0xf5, 0xf0, 0xc0, 0x72, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x23, 0x00, 0x04, 0x22, 0xd7, 0xe6, 0xa4, 0xc0, 0x72, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x23, 0x00, 0x04, 0x36, 0x95, 0x5e, 0x45
    };
    const uint32_t ulExpectedAddress = 0xf0f5d622;
    uint8_t ucBadDnsResponseA[] =
    {
        0x3b, 0x6a, 0x81, 0x83, 0x01, 0x80, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x09, 0x69, 0x6e, 0x73,
        0x70, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x08, 0x75, 0x73, 0x2d, 0x77, 0x65, 0x73, 0x74, 0x32, 0x09,
        0x61, 0x6d, 0x61, 0x7a, 0x6f, 0x6e, 0x61, 0x77, 0x73, 0x03, 0x63, 0x6f, 0x6d, 0x00, 0x00, 0x01,
        0x00, 0x01, 0xc0, 0x1f, 0x00, 0x06, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x33, 0x0e, 0x64,
        0x6e, 0x73, 0x2d, 0x64, 0x79, 0x6e, 0x2d
    };
    uint8_t ucBadDnsResponseB[] =
    {
        0xf0, 0x23, 0x81, 0x80, 0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x03, 0x77, 0x77, 0x77,
        0x05, 0x61, 0x70, 0x70, 0x6c, 0x65, 0x03, 0x63, 0x6f, 0x6d, 0x00, 0x00, 0x01, 0x00, 0x01, 0xc0,
        0x0c, 0x00, 0x05, 0x00, 0x01, 0x00, 0x00, 0x01, 0x7c, 0x00, 0x1b, 0x03, 0x77, 0x77, 0x77, 0x05,
        0x61, 0x70, 0x70, 0x6c, 0x65, 0x03, 0x63, 0x6f, 0x6d, 0x07, 0x65, 0x64, 0x67, 0x65, 0x6b, 0x65,
        0x79, 0x03, 0x6e, 0x65, 0x74, 0x00, 0xc0, 0x2b, 0x00, 0x05, 0x00, 0x01, 0x00, 0x00, 0x1a, 0xd5,
        0x00, 0x2f, 0x03, 0x77, 0x77, 0x77, 0x05, 0x61, 0x70, 0x70, 0x6c, 0x65, 0x03, 0x63, 0x6f, 0x6d,
        0x07, 0x65, 0x64, 0x67, 0x65, 0x6b, 0x65, 0x79, 0x03, 0x6e, 0x65, 0x74, 0x0b, 0x67, 0x6c, 0x6f,
        0x62, 0x61, 0x6c, 0x72, 0x65, 0x64, 0x69, 0x72, 0x06, 0x61, 0x6b, 0x61, 0x64, 0x6e, 0x73, 0xc0,
        0x41, 0xc0, 0x52, 0x00, 0x05, 0x00, 0x01, 0x00, 0x00, 0x08, 0xb3, 0x00, 0x19, 0x05, 0x65, 0x36,
        0x38, 0x35, 0x38, 0x05, 0x64, 0x73, 0x63, 0x65, 0x39, 0x0a, 0x61, 0x6b, 0x61, 0x6d, 0x61, 0x69,
        0x65, 0x64, 0x67, 0x65, 0xc0, 0x41, 0xc0, 0x8d, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0a,
        0x00, 0x04, 0x17, 0x4a, 0x3e, 0x96
    };
    uint8_t ucBadDnsResponseC[] =
    {
        0x3b, 0xa3, 0x81, 0x80, 0x00, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x77, 0x77, 0x77,
        0x09, 0x6d, 0x69, 0x63, 0x72, 0x6f, 0x73, 0x6f, 0x66
    };
    uint8_t ucBadDnsResponseD[] =
    {
        0x95, 0x1e, 0x81, 0x80, 0x05, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x03, 0x77, 0x77, 0x77,
        0x03, 0x63, 0x6e, 0x6e, 0x03, 0x63, 0x6f, 0x6d, 0x00, 0x00, 0x01, 0x00, 0x01, 0xc0, 0x0c, 0x00,
        0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0x48, 0x00, 0x1b, 0x0a, 0x74, 0x75, 0x72, 0x6e, 0x65, 0x72,
        0x2d, 0x74, 0x6c, 0x73, 0x03, 0x6d, 0x61, 0x70, 0x06, 0x66, 0x61, 0x73, 0x74, 0x6c, 0x79, 0x03,
        0x6e, 0x65, 0x74, 0x00, 0xc0, 0x29, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x18, 0x00, 0x04,
        0x97, 0x65, 0x35, 0x43
    };
    uint8_t ucBadDnsResponseE[] =
    {
        0xa8, 0x6d, 0x81, 0x80, 0x03, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x03, 0x77, 0x77, 0x77,
        0x05, 0x61, 0x70, 0x70, 0x6c, 0x65, 0x03, 0x63, 0x6f, 0x6d, 0x00, 0x00, 0x01, 0x00, 0x01, 0xc0,
        0x0c, 0x00, 0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x03, 0x77, 0x77, 0x77, 0x05,
        0x61, 0x70, 0x70, 0x6c, 0x65, 0x03, 0x63, 0x6f, 0x6d, 0x07, 0x65, 0x64, 0x67, 0x65, 0x6b, 0x65,
        0x79, 0x03, 0x6e, 0x65, 0x74, 0x00, 0xc0, 0x2b, 0x00, 0x05, 0x00, 0x01, 0x00, 0x00, 0x1c, 0x2c,
        0x00, 0x2f, 0x03, 0x77, 0x77, 0x77, 0x05, 0x61, 0x70, 0x70, 0x6c, 0x65, 0x03, 0x63, 0x6f, 0x6d,
        0x07, 0x65, 0x64, 0x67, 0x65, 0x6b, 0x65, 0x79, 0x03, 0x6e, 0x65, 0x74, 0x0b, 0x67, 0x6c, 0x6f,
        0x62, 0x61, 0x6c, 0x72, 0x65, 0x64, 0x69, 0x72, 0x06, 0x61, 0x6b, 0x61, 0x64, 0x6e, 0x73, 0xc0,
        0x41, 0xc0, 0x52, 0x00, 0x05, 0x00, 0x01, 0x00, 0x00, 0x03, 0xd7, 0x00, 0x19, 0x05, 0x65, 0x36,
        0x38, 0x35, 0x38, 0x05, 0x64, 0x73, 0x63, 0x65, 0x39, 0x0a, 0x61, 0x6b, 0x61, 0x6d, 0x61, 0x69,
        0x65, 0x64, 0x67, 0x65, 0xc0, 0x41, 0xc0, 0x8d, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08,
        0x00, 0x04, 0x17, 0x4b, 0xba, 0x13
    };
    uint8_t ucBadDnsResponseF[] =
    {
        0x6c, 0x1e, 0x81, 0x80, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x77, 0x77, 0x77,
        0x06, 0x61, 0x6d, 0x61, 0x7a, 0x6f, 0x6e, 0x03, 0x63, 0x6f, 0x6d, 0x00, 0x00, 0x01, 0x00, 0x01,
        0xc0, 0x0c, 0x00, 0x05, 0x00, 0x01, 0x00, 0x00, 0x01, 0x57, 0x00, 0x0a, 0x03, 0x77, 0x77, 0x77,
        0x03, 0x63, 0x64, 0x6e, 0xc0, 0x10, 0x41, 0x41, 0xc0, 0x2c, 0x00, 0x05, 0x00, 0x01, 0x00, 0x00,
        0x00, 0x17, 0x00, 0x1f, 0x0e, 0x64, 0x33, 0x61, 0x67, 0x34, 0x68, 0x75, 0x6b, 0x6b, 0x68, 0x36,
        0x32, 0x79, 0x6e, 0x0a, 0x63, 0x6c, 0x6f, 0x75, 0x64, 0x66, 0x72, 0x6f, 0x6e, 0x74, 0x03, 0x6e,
        0x65, 0x74, 0x00, 0xc0, 0x42, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x2e, 0x00, 0x04, 0x0d,
        0x20, 0xa7, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x9a, 0x3a, 0x01, 0x5c, 0x31, 0x1f, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00
    };
    uint8_t ucBadDnsResponseG[] =
    {
        0x73, 0xe1, 0x81, 0x80, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x77, 0x77, 0x77,
        0x06, 0x22, 0x03, 0x63, 0x6f, 0x6d, 0x00, 0x00, 0x01, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x01, 0x00,
        0x01, 0x00, 0x00, 0x01, 0x0a, 0x00, 0x04, 0xd8, 0x3a, 0xd8, 0x84, 0x00, 0x34, 0x02, 0x41, 0x01,
        0x2c, 0xb2, 0x1a, 0x01, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00
    };
    uint8_t ucBadDnsResponseH[] = /* Regress crash in prvReadNameField. */
    {
        0x10, 0x00, 0x00, 0x35, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x35,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x91, 0x91, 0x91,
        0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
        0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
        0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
        0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
        0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
        0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
        0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
        0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
        0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    uint8_t ucBadDnsResponseI[] = /* Regress crash in prvSkipNameField. */
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8,
        0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8,
        0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0x00, 0x35,
        0x0a, 0xf8, 0xf8, 0xf8, 0x27, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8,
        0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0x16, 0x16, 0x21, 0x16,
        0x16, 0x16, 0x16, 0x16, 0x16, 0x2a, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xf8,
        0x27, 0xf8, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
        0x00, 0x16, 0x16, 0x16, 0x16, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x16, 0x5a,
        0x00, 0x16, 0x00, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x21
    };
    uint32_t ulAddress = 0;

    /* Parsing a valid packet should succeed. */
    ulAddress = TEST_FreeRTOS_TCP_prvParseDNSReply(
        ucGoodDnsResponse,
        sizeof( ucGoodDnsResponse ),
        *( uint16_t * ) ucGoodDnsResponse );
    TEST_ASSERT_EQUAL_UINT32( ulExpectedAddress, ulAddress );
    /* End test. */

    /* Parsing a bad packet should fail gracefully. */
    ulAddress = TEST_FreeRTOS_TCP_prvParseDNSReply(
        ucBadDnsResponseA,
        sizeof( ucBadDnsResponseA ),
        *( uint16_t * ) ucBadDnsResponseA );
    TEST_ASSERT_EQUAL_UINT32( 0, ulAddress );
    /* End test. */

    /* Parsing a bad packet should fail gracefully. */
    ulAddress = TEST_FreeRTOS_TCP_prvParseDNSReply(
        ucBadDnsResponseB,
        sizeof( ucBadDnsResponseB ),
        *( uint16_t * ) ucBadDnsResponseB );
    TEST_ASSERT_EQUAL_UINT32( 0, ulAddress );
    /* End test. */

    /* Parsing a bad packet should fail gracefully. */
    ulAddress = TEST_FreeRTOS_TCP_prvParseDNSReply(
        ucBadDnsResponseC,
        sizeof( ucBadDnsResponseC ),
        *( uint16_t * ) ucBadDnsResponseC );
    TEST_ASSERT_EQUAL_UINT32( 0, ulAddress );
    /* End test. */

    /* Parsing a bad packet should fail gracefully. */
    ulAddress = TEST_FreeRTOS_TCP_prvParseDNSReply(
        ucBadDnsResponseD,
        sizeof( ucBadDnsResponseD ),
        *( uint16_t * ) ucBadDnsResponseD );
    TEST_ASSERT_EQUAL_UINT32( 0, ulAddress );
    /* End test. */

    /* Parsing a bad packet should fail gracefully. */
    ulAddress = TEST_FreeRTOS_TCP_prvParseDNSReply(
        ucBadDnsResponseE,
        sizeof( ucBadDnsResponseE ),
        *( uint16_t * ) ucBadDnsResponseE );
    TEST_ASSERT_EQUAL_UINT32( 0, ulAddress );
    /* End test. */

    /* Parsing a bad packet should fail gracefully. */
    ulAddress = TEST_FreeRTOS_TCP_prvParseDNSReply(
        ucBadDnsResponseF,
        sizeof( ucBadDnsResponseF ),
        *( uint16_t * ) ucBadDnsResponseF );
    TEST_ASSERT_EQUAL_UINT32( 0, ulAddress );
    /* End test. */

    /* Parsing a bad packet should fail gracefully. */
    ulAddress = TEST_FreeRTOS_TCP_prvParseDNSReply(
        ucBadDnsResponseG,
        sizeof( ucBadDnsResponseG ),
        *( uint16_t * ) ucBadDnsResponseG );
    TEST_ASSERT_EQUAL_UINT32( 0, ulAddress );
    /* End test. */

    /* Parsing a bad packet should fail gracefully. */
    ulAddress = TEST_FreeRTOS_TCP_prvParseDNSReply(
        ucBadDnsResponseH,
        sizeof( ucBadDnsResponseH ),
        *( uint16_t * ) ucBadDnsResponseH );
    TEST_ASSERT_EQUAL_UINT32( 0, ulAddress );
    /* End test. */

    /* Parsing a bad packet should fail gracefully. */
    ulAddress = TEST_FreeRTOS_TCP_prvParseDNSReply(
        ucBadDnsResponseI,
        sizeof( ucBadDnsResponseI ),
        *( uint16_t * ) ucBadDnsResponseI );
    TEST_ASSERT_EQUAL_UINT32( 0, ulAddress );
    /* End test. */
}

TEST( Full_FREERTOS_TCP, ulDNSHandlePacket )
{
    NetworkBufferDescriptor_t xNetworkBuffer = { 0 };
    uint8_t ucPartialUdpPacket[ sizeof( ipSIZE_OF_UDP_HEADER ) - 1 ] = { 0xFF };
    uint32_t ulResult = 0;

    /* Attempting to parse a packet that's shorter than a UDP header should be
     * a no-op. */
    xNetworkBuffer.pucEthernetBuffer = ucPartialUdpPacket;
    xNetworkBuffer.xDataLength = sizeof( ucPartialUdpPacket );
    ulResult = ulDNSHandlePacket( &xNetworkBuffer );
    TEST_ASSERT_EQUAL_UINT32( 0, ulResult );
}

TEST( Full_FREERTOS_TCP, prvCheckOptions )
{
    uint8_t ucDivideByZero[] =
    {
        0x6f, 0xff, 0xff, 0xff, 0x0a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xde, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6f, 0x6f, 0x6f, 0x6d,
        0x6f, 0xff, 0xff, 0xff, 0x0a, 0xff, 0xff, 0xff, 0xff, 0xe5, 0x6f, 0x6f,
        0x6f, 0x6f, 0x6f, 0x6b, 0xbf, 0x6f, 0x03, 0xff, 0x04, 0x01, 0xb7, 0xff,
        0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x04, 0x02, 0x02, 0x02, 0x02,
        0x02, 0x02, 0x02, 0x02, 0x02, 0x04, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02,
        0x02, 0x02, 0xf8, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x05
    };

    FreeRTOS_Socket_t xSocket;
    NetworkBufferDescriptor_t xNetworkBuffer;

    xNetworkBuffer.pucEthernetBuffer = ucDivideByZero;
    xNetworkBuffer.xDataLength = sizeof( ucDivideByZero );

    TEST_FreeRTOS_TCP_prvTCPCreateWindow( &xSocket );
    TEST_FreeRTOS_TCP_prvCheckOptions( &xSocket, &xNetworkBuffer );
}

TEST( Full_FREERTOS_TCP, UDPPacketLength )
{
    uint8_t ucBadUdpPacketA[] =
    {
        0xff, 0xff
    };

    uint8_t ucBadUdpPacketB[] =
    {
        0x0a, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0x54, 0xbf, 0xbf, 0xbf, 0xff, 0xbf,
        0x0a, 0xbf, 0xbf, 0xbf, 0x3f, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf,
        0x88, 0x00, 0xbf, 0xbf, 0xbf, 0x00, 0x32, 0xbf, 0xbf, 0xbf, 0x00, 0x35,
        0x0a, 0xbf, 0xbf, 0x3a, 0xbf, 0xbf, 0xbf
    };

    BaseType_t xReturn = pdPASS;
    uint16_t usPort = 65535;
    NetworkBufferDescriptor_t xNetworkBuffer;

    /* This test fails now since there is an assert
     * checking for NULL pucEthernetBuffer. Also, the
     * next tests do not run and this whole test case
     * is scrapped.
     *
     * xNetworkBuffer.pucEthernetBuffer = NULL;
     * xNetworkBuffer.xDataLength = 0;
     *
     * xReturn = xProcessReceivedUDPPacket( &xNetworkBuffer, usPort );
     * TEST_ASSERT_EQUAL_UINT32_MESSAGE( pdFAIL, xReturn, "Failed to parse 0 size packet" );
     */


    xNetworkBuffer.pucEthernetBuffer = ucBadUdpPacketA;
    xNetworkBuffer.xDataLength = sizeof( ucBadUdpPacketA );
    xReturn = xProcessReceivedUDPPacket( &xNetworkBuffer, usPort );
    TEST_ASSERT_EQUAL_UINT32_MESSAGE( pdFAIL, xReturn, "Failed to parse 2 bytes packet" );

    xNetworkBuffer.pucEthernetBuffer = ucBadUdpPacketB;
    xNetworkBuffer.xDataLength = sizeof( ucBadUdpPacketB );
    xReturn = xProcessReceivedUDPPacket( &xNetworkBuffer, usPort );
    TEST_ASSERT_EQUAL_UINT32( pdFAIL, xReturn );
}
