/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EMBEDDED_RPC__ERROR_HANDLER_H_
#define _EMBEDDED_RPC__ERROR_HANDLER_H_

#include "erpc_common.h"
#include <stdint.h>

/*!
 * @addtogroup error_handler
 * @{
 * @file
 */

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

//! @name Error handler
//@{

/*!
 * @brief Handle an eRPC status error.
 *
 * This function prints a description of the occurred error and sets
 * g_erpc_error_occurred for client-side sample logic that wants to observe
 * whether an eRPC error happened.
 *
 * @param err eRPC status value to report.
 * @param functionID Function identifier related to the error, or 0 when no
 * function-specific context is available.
 */
void erpc_error_handler(erpc_status_t err, uint32_t functionID);

//@}

#ifdef __cplusplus
}
#endif

/*! @} */
#endif /* _EMBEDDED_RPC__ERROR_HANDLER_H_ */
