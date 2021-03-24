/***************************************************************************//**
* @file sl_btmesh_data_logging_server.h
* @brief BT Mesh Data Logging Server Header
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* SPDX-License-Identifier: Zlib
*
* The licensor of this software is Silicon Laboratories Inc.
*
* This software is provided \'as-is\', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
*******************************************************************************
* # Experimental Quality
* This code has not been formally tested and is provided as-is. It is not
* suitable for production environments. In addition, this code will not be
* maintained and there may be no bug maintenance planned for these resources.
* Silicon Labs may update projects from time to time.
******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef SL_BTMESH_DATA_LOGGING_SERVER_H
#define SL_BTMESH_DATA_LOGGING_SERVER_H

#include "em_common.h"
#include "sl_btmesh_data_logging_capi.h"

/// Data sending length type
typedef uint16_t sl_data_log_length_t;

sl_status_t sl_btmesh_data_log_server_init(void);

void sl_btmesh_data_log_ovf_callback(void);

void sl_btmesh_data_log_full_callback(void);

void sl_btmesh_data_log_complete_callback(void);
sl_status_t sl_btmesh_data_log_server_send_status(void);
sl_status_t sl_btmesh_data_log_on_server_receive_event(
                            sl_btmesh_msg_t *evt);
sl_status_t sl_btmesh_data_log_append(sl_data_log_data_t *data);

sl_status_t sl_btmesh_data_log_server_start(void);

void sl_btmesh_data_log_on_sample_callback(void);

sl_btmesh_data_log_raw_t sl_btmesh_data_log_get_raw(void);

void sl_btmesh_data_log_on_properties_callback(void);

#endif // SL_BTMESH_DATA_LOGGING_SERVER_H

#ifdef __cplusplus
}
#endif
