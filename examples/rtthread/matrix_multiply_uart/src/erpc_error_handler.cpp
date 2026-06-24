/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "erpc_error_handler.h"
#include <rtthread.h>

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

extern bool g_erpc_error_occurred;
bool g_erpc_error_occurred = false;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

void erpc_error_handler(erpc_status_t err, uint32_t functionID)
{
    switch (err)
    {
        case kErpcStatus_Fail:
            rt_kprintf("\r\nGeneric failure.");
            break;

        case kErpcStatus_InvalidArgument:
            rt_kprintf("\r\nArgument is an invalid value.");
            break;

        case kErpcStatus_Timeout:
            rt_kprintf("\r\nOperated timed out.");
            break;

        case kErpcStatus_InvalidMessageVersion:
            rt_kprintf("\r\nMessage header contains an unknown version.");
            break;

        case kErpcStatus_ExpectedReply:
            rt_kprintf("\r\nExpected a reply message but got another message type.");
            break;

        case kErpcStatus_CrcCheckFailed:
            rt_kprintf("\r\nMessage is corrupted.");
            break;

        case kErpcStatus_BufferOverrun:
            rt_kprintf("\r\nAttempt to read or write past the end of a buffer.");
            break;

        case kErpcStatus_UnknownName:
            rt_kprintf("\r\nCould not find host with given name.");
            break;

        case kErpcStatus_ConnectionFailure:
            rt_kprintf("\r\nFailed to connect to host.");
            break;

        case kErpcStatus_ConnectionClosed:
            rt_kprintf("\r\nConnected closed by peer.");
            break;

        case kErpcStatus_MemoryError:
            rt_kprintf("\r\nMemory allocation error.");
            break;

        case kErpcStatus_ServerIsDown:
            rt_kprintf("\r\nServer is stopped.");
            break;

        case kErpcStatus_InitFailed:
            rt_kprintf("\r\nTransport layer initialization failed.");
            break;

        case kErpcStatus_ReceiveFailed:
            rt_kprintf("\r\nFailed to receive data.");
            break;

        case kErpcStatus_SendFailed:
            rt_kprintf("\r\nFailed to send data.");
            break;

        /* no error occurred */
        case kErpcStatus_Success:
            return;

        /* unhandled error */
        default:
            rt_kprintf("\r\nUnhandled error occurred.");
            break;
    }

    /* When error occurred on client side. */
    if (functionID != 0U)
    {
        rt_kprintf("Function id '%u'.", (unsigned int)functionID);
    }
    rt_kprintf("\r\n");

    /* error occurred */
    g_erpc_error_occurred = true;
}
