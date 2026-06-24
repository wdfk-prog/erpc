/*
 * Copyright 2026
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <rtthread.h>

#include "erpc_server_setup.h"
#include "erpc_transport_setup.h"
#include "c_erpc_matrix_multiply_server.h"
#include "erpc_matrix_multiply_common.h"
#include "erpc_error_handler.h"

#ifndef PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_DEV_NAME
#define PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_DEV_NAME "uart2"
#endif

#ifndef PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_BAUDRATE
#define PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_BAUDRATE 115200
#endif

#ifndef PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_THREAD_STACK_SIZE
#define PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_THREAD_STACK_SIZE 4096
#endif

#ifndef PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_THREAD_PRIORITY
#define PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_THREAD_PRIORITY 20
#endif

#ifndef PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_THREAD_TIMESLICE
#define PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_THREAD_TIMESLICE 10
#endif

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

static rt_thread_t s_erpc_matrix_thread = RT_NULL;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Matrix multiply RPC implementation.
 *
 * This function is called by the generated eRPC server shim when the PC client
 * invokes the MatrixMultiplyService::erpcMatrixMultiply RPC.
 *
 * @param matrix1 First input matrix.
 * @param matrix2 Second input matrix.
 * @param result_matrix Output matrix.
 */
void erpcMatrixMultiply(Matrix matrix1, Matrix matrix2, Matrix result_matrix)
{
    int32_t i, j, k;

    /* Clear the result matrix. */
    for (i = 0; i < matrix_size; ++i)
    {
        for (j = 0; j < matrix_size; ++j)
        {
            result_matrix[i][j] = 0;
        }
    }

    /* Multiply two matrices. */
    for (i = 0; i < matrix_size; ++i)
    {
        for (j = 0; j < matrix_size; ++j)
        {
            for (k = 0; k < matrix_size; ++k)
            {
                result_matrix[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

/*!
 * @brief Run the eRPC matrix multiply UART server thread.
 *
 * @param parameter Thread parameter, unused.
 */
static void erpc_matrix_multiply_uart_thread_entry(void *parameter)
{
    erpc_transport_t transport = RT_NULL;
    erpc_mbf_t message_buffer_factory = RT_NULL;
    erpc_server_t server = RT_NULL;
    erpc_service_t service = RT_NULL;
    erpc_status_t status;

    RT_UNUSED(parameter);

    transport = erpc_transport_rtthread_uart_init(PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_DEV_NAME,
                                                  PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_BAUDRATE);
    if (transport == RT_NULL)
    {
        rt_kprintf("eRPC UART transport init failed: %s\r\n", PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_DEV_NAME);
        goto exit_thread;
    }

    message_buffer_factory = erpc_mbf_dynamic_init();
    if (message_buffer_factory == RT_NULL)
    {
        rt_kprintf("eRPC dynamic message buffer factory init failed\r\n");
        goto exit_thread;
    }

    server = erpc_server_init(transport, message_buffer_factory);
    if (server == RT_NULL)
    {
        rt_kprintf("eRPC server init failed\r\n");
        goto exit_thread;
    }

    service = create_MatrixMultiplyService_service();
    if (service == RT_NULL)
    {
        rt_kprintf("eRPC matrix multiply service init failed\r\n");
        goto exit_thread;
    }

    erpc_add_service_to_server(server, service);

    rt_kprintf("eRPC matrix multiply UART server started: %s, %d\r\n",
               PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_DEV_NAME,
               PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_BAUDRATE);

    while (1)
    {
        status = erpc_server_poll(server);
        if (status != kErpcStatus_Success)
        {
            erpc_error_handler(status, 0U);
            break;
        }

        rt_thread_mdelay(1);
    }

exit_thread:
    if ((server != RT_NULL) && (service != RT_NULL))
    {
        erpc_remove_service_from_server(server, service);
    }

    if (service != RT_NULL)
    {
        destroy_MatrixMultiplyService_service(service);
    }

    if (server != RT_NULL)
    {
        erpc_server_stop(server);
        erpc_server_deinit(server);
    }

    if (message_buffer_factory != RT_NULL)
    {
        erpc_mbf_dynamic_deinit(message_buffer_factory);
    }

    if (transport != RT_NULL)
    {
        erpc_transport_rtthread_uart_deinit(transport);
    }

    s_erpc_matrix_thread = RT_NULL;
    rt_kprintf("eRPC matrix multiply UART server stopped\r\n");
}

/*!
 * @brief Automatically start the eRPC matrix multiply UART example server.
 *
 * @return RT_EOK on success, otherwise negative errno.
 */
extern "C" int erpc_matrix_multiply_uart_auto_start(void)
{
    rt_err_t result;

    if (s_erpc_matrix_thread != RT_NULL)
    {
        return RT_EOK;
    }

    s_erpc_matrix_thread = rt_thread_create("erpc_mm",
                                            erpc_matrix_multiply_uart_thread_entry,
                                            RT_NULL,
                                            PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_THREAD_STACK_SIZE,
                                            PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_THREAD_PRIORITY,
                                            PKG_ERPC_EXAMPLE_MATRIX_MULTIPLY_UART_THREAD_TIMESLICE);
    if (s_erpc_matrix_thread == RT_NULL)
    {
        rt_kprintf("create eRPC matrix multiply UART thread failed\r\n");
        return -RT_ERROR;
    }

    result = rt_thread_startup(s_erpc_matrix_thread);
    if (result != RT_EOK)
    {
        rt_kprintf("startup eRPC matrix multiply UART thread failed: %d\r\n", result);
        rt_thread_delete(s_erpc_matrix_thread);
        s_erpc_matrix_thread = RT_NULL;
        return result;
    }

    return RT_EOK;
}
INIT_APP_EXPORT(erpc_matrix_multiply_uart_auto_start);
