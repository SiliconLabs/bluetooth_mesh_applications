/***************************************************************************//**
* @file sl_btmesh_data_logging_config.h
* @brief BT Mesh Data Logging configuration Header
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

#ifndef SL_BTMESH_DATA_LOGGING_CONFIG_H
#define SL_BTMESH_DATA_LOGGING_CONFIG_H

#include "sl_btmesh_device_properties.h"

/// The Data Log buffer size
#define SL_BTMESH_DATA_LOG_BUFF_SIZE_CFG_VAL  ((size_t)100)

/// Default Main element
#define SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL    ((uint16_t)0)

/// Enable buffer roll over
//#define SL_BTMESH_DATA_LOG_BUFF_ROLL_EN

/// Timeout value in MS
#define SL_BTMESH_DATA_LOG_TIMEOUT_MS_CFG_VAL       ((uint16_t)3000)
/// Time delay value for send response in MS
#define SL_BTMESH_DATA_LOG_RESP_MS_CFG_VAL          ((uint16_t)10)
/// Send delay timer value in MS
#define SL_BTMESH_DATA_LOG_SEND_DELAY_MS_CFG_VAL    ((uint16_t)10)

/// Sample rate in MS
#define SL_BTMESH_DATA_LOG_SAMPLE_RATE_MS_CFG_VAL   ((uint16_t)1000)
/// Log period in MS
#define SL_BTMESH_DATA_LOG_PERIOD_MS_CFG_VAL        ((uint16_t)10000)
/// Threshold value
#define SL_BTMESH_DATA_LOG_THESHOLD_CFG_VAL  ((sl_btmesh_data_log_threshold_t)0)

/// The logging data type
typedef struct {
  temperature_8_t temp;
  percentage_8_t  humid;
}sl_data_log_data_t;

#endif // SL_BTMESH_DATA_LOGGING_CONFIG_H

#ifdef __cplusplus
}
#endif
