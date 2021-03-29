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

#ifndef SL_BTMESH_DATA_LOGGING_CONFIG_H
#define SL_BTMESH_DATA_LOGGING_CONFIG_H

/// The Data Log buffer size
#define SL_BTMESH_DATA_LOG_BUFF_SIZE_CFG_VAL  100

/// Default Main element
#define SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL    0

/// Enable buffer roll over
#define SL_BTMESH_DATA_LOG_BUFF_ROLL_EN

/// Timeout value in MS
#define SL_BTMESH_DATA_LOG_TIMEOUT_MS_CFG_VAL   (3000)
/// Time delay value for send response in MS
#define SL_BTMESH_DATA_LOG_RESP_MS_CFG_VAL      (1)

/// Sample rate in MS
#define SL_BTMESH_DATA_LOG_SAMPLE_RATE_MS_CFG_VAL   (1000)
/// Log period in MS
#define SL_BTMESH_DATA_LOG_PERIOD_MS_CFG_VAL        (5000)
/// Threshold value
#define SL_BTMESH_DATA_LOG_RAW_CFG_VAL              (1)

/// The logging data type
typedef uint8_t sl_data_log_data_t;

#endif // SL_BTMESH_DATA_LOGGING_CONFIG_H

#ifdef __cplusplus
}
#endif
