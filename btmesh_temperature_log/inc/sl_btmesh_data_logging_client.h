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

#define SL_BTMESH_RECV_ID_INIT_VAL    0x55

/***************************************************************************//**
 * Initialize the data log client.
 *
 * @param[in] data_log_ptr Pointer to Log data instance.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_client_init(sl_data_log_recv_t *data_log_ptr);

/***************************************************************************//**
 * De-Initialize the data log client.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_client_deinit(void);

/***************************************************************************//**
 * Handle event of the Log client.
 *
 * @param[in] evt Pointer to btmesh message.
 *
 * Handle event of the Vendor model. If there's valid opcode received then
 * receiving handler will be called.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 *  @note This function must be executed in sl_btmesh_on_event for the
 *  sl_btmesh_evt_vendor_model_receive_id event.
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_on_client_receive_event( sl_btmesh_msg_t *evt);

/***************************************************************************//**
 * Send request to update Log period to server.
 *
 * @param[in] period Pointer to period data type instance.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_client_set_period(
                sl_btmesh_data_log_period_t *period);

/***************************************************************************//**
 * Send request to update sample rate to server.
 *
 * @param[in] rate Pointer to sample rate data type instance.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_client_set_sample_rate(
                sl_btmesh_data_log_sample_rate_t *rate);

/***************************************************************************//**
 * Send request to update sample threshold to server.
 *
 * @param[in] threshold Pointer to threshold data type instance.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_client_set_threshold(
                sl_btmesh_data_log_threshold_t *threshold);

/***************************************************************************//**
 * Receive complete callback function.
 *
 ******************************************************************************/
void sl_btmesh_data_log_client_recv_complete_callback(void);

/***************************************************************************//**
 * Data receive complete callback function.
 *
 ******************************************************************************/
void sl_btmesh_data_log_client_data_recv_callback(
                               sl_data_log_data_t *data);

/***************************************************************************//**
 * Reset the Log.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_client_reset_log(void);

/***************************************************************************//**
 * Get the current status of the Log.
 *
 * @return Return the current status of the Log.
 *  - SL_BTMESH_DATA_LOG_IDLE if Log is idle.
 *  - SL_BTMESH_DATA_LOG_BUSY if Log is receiving.
 *  - SL_BTMESH_DATA_LOG_COMPLETE if Log is received.
 *
 ******************************************************************************/
uint8_t sl_btmesh_data_log_get_client_state(void);

#endif // SL_BTMESH_DATA_LOGGING_CLIENT_H

#ifdef __cplusplus
}
#endif
