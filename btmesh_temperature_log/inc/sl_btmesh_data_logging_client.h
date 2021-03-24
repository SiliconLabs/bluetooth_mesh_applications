/***************************************************************************//**
* @file sl_btmesh_data_logging_client.h
* @brief BT Mesh Data Logging Client Header
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

#ifndef SL_BTMESH_DATA_LOGGING_CLIENT_H
#define SL_BTMESH_DATA_LOGGING_CLIENT_H

#include "sl_btmesh_data_logging_capi.h"

/// The "Last" flag in receive array
#define SL_BTMESH_BYTE_FLAG_POS     0
#define SL_BTMESH_BYTE_FLAG_LEN     1

sl_status_t sl_btmesh_data_log_client_init(sl_data_log_t *data_log_ptr);
void sl_btmesh_data_log_client_recv_complete_callback(void);
sl_status_t sl_btmesh_data_log_client_deinit(void);
sl_status_t sl_btmesh_data_log_on_client_receive_event(
        sl_btmesh_msg_t *evt);
uint8_t sl_btmesh_data_log_get_client_state(void);
sl_status_t sl_btmesh_data_log_client_set_period(
                sl_btmesh_data_log_period_t *period);
sl_status_t sl_btmesh_data_log_client_set_sample_rate(
                sl_btmesh_data_log_sample_rate_t *rate);
#endif // SL_BTMESH_DATA_LOGGING_CLIENT_H

#ifdef __cplusplus
}
#endif
