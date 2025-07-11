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
 * @file cellular_config.h
 * @brief cellular config options.
 */

#ifndef __CELLULAR_CONFIG_H__
#define __CELLULAR_CONFIG_H__

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Include logging header files and define logging macros in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define the LIBRARY_LOG_NAME and LIBRARY_LOG_LEVEL macros depending on
 * the logging configuration for DEMO.
 * 3. Include the header file "logging_stack.h", if logging is enabled for DEMO.
 */

#include "logging_levels.h"

/* Logging configuration for the Demo. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME    "CellularLib"
#endif

#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_INFO
#endif

/* Prototype for the function used to print to console on Windows simulator
 * of FreeRTOS.
 * The function prints to the console before the network is connected;
 * then a UDP port after the network has connected. */
extern void vLoggingPrintf( const char * pcFormatString,
                            ... );

/* Map the SdkLog macro to the logging function to enable logging
 * on Windows simulator. */
#ifndef SdkLog
    #define SdkLog( message )    vLoggingPrintf message
#endif

#include "logging_stack.h"

/************ End of logging configuration ****************/

/* This is a project specific file and is used to override config values defined
 * in cellular_config_defaults.h. */

/**
 * Cellular comm interface make use of COM port on computer to communicate with
 * cellular module on windows simulator, for example "COM5".
 * #define CELLULAR_COMM_INTERFACE_PORT    "...insert here..."
 */

/*
 * Default APN for network registration.
 * #define CELLULAR_APN                    "...insert here..."
 */

/*
 * PDN context id for cellular network.
 */
#define CELLULAR_PDN_CONTEXT_ID         ( CELLULAR_PDN_CONTEXT_ID_MIN )

/*
 * PDN connect timeout for network registration.
 */
#define CELLULAR_PDN_CONNECT_TIMEOUT    ( 100000UL )

/*
 * Overwrite default config for different cellular modules.
 */

/*
 * GetHostByName API is not used in the demo. IP address is used to store the hostname.
 * The value should be longer than the length of democonfigMQTT_BROKER_ENDPOINT in demo_config.h.
 */
#define CELLULAR_IP_ADDRESS_MAX_SIZE    ( 64U )

/*
 * Sara R4 maximum socket send buffer size is 1024U.
 */
#define CELLULAR_MAX_SEND_DATA_LEN      ( 1024U )

/*
 * Sara R4 maximum socket receive buffer size is 1024U.
 */
#define CELLULAR_MAX_RECV_DATA_LEN      ( 1024U )

/*
 * Sara R4 supports set mobile network operators commands.
 * Set the mobile network operators of your environment.
 * Reference https://www.u-blox.com/sites/default/files/SARA-R4_ATCommands_%28UBX-17003787%29.pdf
 * 0: undefined / regulatory
 * 1: SIM ICCID/IMSI select
 * 2: AT&T
 * 3: Verizon
 * 4: Telstra
 * 5: T-Mobile US
 * 6: China Telecom
 * 8: Sprint
 * 19: Vodafone
 * 20: NTT DoCoMo
 * 21: Telus
 * 28: SoftBank
 * 31: Deutsche Telekom
 * 32: US Cellular
 * 33: VIVO
 * 39: SKT
 * 44: Claro Brasil
 * 45: TIM Brasil
 * 46: Orange France
 * 90: global
 * 100: standard Europe
 * Notes :
 * The standard Europe profile should be used as the basis for all other MNOs in
 * Europe outside of Vodafone and Deutsche Telekom. However, there may be changes
 * that need to be applied to the module for properoperation with any given European
 * MNO such as attach type, RAT preference, band selection, etc. Pleaseconsult with
 * the preferred network provider.
 * #define CELLULAR_CONFIG_SARA_R4_SET_MNO_PROFILE         ( ...insert here... )
 */

/*
 * Reboot Sara R4 cellular modem in module init function.
 */
#define  CELLULAR_CONFIG_SARA_R4_REBOOT_ON_INIT

#endif /* __CELLULAR_CONFIG_H__ */
