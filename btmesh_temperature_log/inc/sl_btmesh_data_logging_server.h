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
*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef SL_BTMESH_DATA_LOGGING_SERVER_H
#define SL_BTMESH_DATA_LOGGING_SERVER_H

#include "em_common.h"
#include "sl_btmesh_data_logging_capi.h"

/// Data sending length type
typedef uint16_t sl_data_log_length_t;

/***************************************************************************//**
 * Initialize the data log server.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_server_init(void);

/***************************************************************************//**
 * De-Initialize the data log server.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_server_deinit(void);

/***************************************************************************//**
 * Start to publish the Log.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_server_send_status(void);

/***************************************************************************//**
 * Reset the Log.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_reset(void);

/***************************************************************************//**
 * Get the current status of the Log.
 *
 * @return Return the current status of the Log.
 *  - SL_BTMESH_DATA_LOG_IDLE if Log is idle.
 *  - SL_BTMESH_DATA_LOG_BUSY if Log is receiving.
 *
 ******************************************************************************/
uint8_t sl_btmesh_data_log_get_server_state(void);

/***************************************************************************//**
 * Handle event of the Log server.
 *
 * @param[in] evt Pointer to btmesh message.
 *
 * Handle event of the Vendor model. If there's valid opcode received then
 * receiving handler will be called for:
 * - Log received response.
 * - Set period request.
 * - Set sample rate request.
 * - Set threshold request.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 *  @note This function must be executed in sl_btmesh_on_event for the
 *  sl_btmesh_evt_vendor_model_receive_id event.
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_on_server_receive_event(
                            sl_btmesh_msg_t *evt);

/***************************************************************************//**
 * Append new data to the Log.
 *
 * @param[in] data Data value to be appended to the Log.
 *
 * If the Log is full then a full callback is executed.
 * If the Log is overflow then a overflow callback is executed.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_append(sl_data_log_data_t *data);

/***************************************************************************//**
 * Start the sample rate timer and Log period timer.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_server_start(void);

/***************************************************************************//**
 * Get the threshold value.
 *
 * @return returns the current threshold value.
 *
 ******************************************************************************/
sl_btmesh_data_log_threshold_t sl_btmesh_data_log_get_threshold(void);

/***************************************************************************//**
 * Check the log sending is started.
 *
 * @return returns the log sending status.
 *         - true if log is requested to send
 *         - false if log is idle
 *
 ******************************************************************************/
bool sl_btmesh_data_log_is_started_sending(void);

/***************************************************************************//**
 * Process log sending.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_step(void);

/***************************************************************************//**
 * Publish the current data.
 *
 * @param[in] data pointer to data to be sent.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_server_send_data(sl_data_log_data_t *data);

/***************************************************************************//**
 * Delete NVM storage.
 *
 ******************************************************************************/
void sl_btmesh_data_log_reset_config(void);

/***************************************************************************//**
 * Weak implementation of Callbacks.
 *
 ******************************************************************************/
void sl_btmesh_data_log_ovf_callback(void);

void sl_btmesh_data_log_full_callback(void);

void sl_btmesh_data_log_complete_callback(void);

void sl_btmesh_data_log_on_sample_callback(void);

void sl_btmesh_data_log_on_periodic_callback(void);

#endif // SL_BTMESH_DATA_LOGGING_SERVER_H

#ifdef __cplusplus
}
#endif
