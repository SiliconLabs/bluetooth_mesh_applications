/***************************************************************************//**
* @file sl_btmesh_data_logging_capi.h
* @brief BT Mesh Data Logging Header
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

#ifndef SL_BTMESH_DATA_LOGGING_CAPI_H
#define SL_BTMESH_DATA_LOGGING_CAPI_H

#include "sl_btmesh_api.h"
#include "sl_btmesh_data_logging_config.h"

/// Silabs company ID
#define SL_BTMESH_VENDOR_ID                         0x02FF

/// Vendor model server ID
#define SL_BTMESH_DATA_LOG_MODEL_SERVER_ID      0x0000

/// Vendor model client ID
#define SL_BTMESH_DATA_LOG_MODEL_CLIENT_ID      0x0001

/// Opcode data length
#define SL_BTMESH_DATA_LOG_OPCODE_LENGTH        5

/// Data Log Status messages ID
#define SL_BTMESH_DATA_LOG_MESSAGE_STATUS_ID          0x01
/// Data Log Status respond messages ID
#define SL_BTMESH_DATA_LOG_MESSAGE_STATUS_RSP_ID      0x02
/// Data Log sending timing
#define SL_BTMESH_DATA_LOG_MESSAGE_PERIOD_ID          0x03
/// Data Log sample timing
#define SL_BTMESH_DATA_LOG_MESSAGE_SAMPLE_RATE_ID     0x04
/// Raw data used for specific purpose
#define SL_BTMESH_DATA_LOG_MESSAGE_RAW_ID             0x05

/// Client response cmd length
#define SL_BTMESH_DATA_LOG_MESSAGE_STATUS_RSP_LEN     1
#define SL_BTMESH_DATA_LOG_MESSAGE_STATUS_RSP_DATA    NULL

/// Maximum data length of a vendor model massage (byte)
#define SL_BTMESH_STACK_SEND_LENGTH_MAX           (247)
/// Handshake information data length (byte)
#define  SL_BTMESH_DATA_LOG_INFO_LENGTH           1
/// Maximum length in one data sending
/// The max length is decreased because of the additional handshake information
#define SL_BTMESH_DATA_LOG_LENGTH_MAX (SL_BTMESH_STACK_SEND_LENGTH_MAX \
                                       - SL_BTMESH_DATA_LOG_INFO_LENGTH)

/// Number of data index sent in one publication
#define SL_BTMESH_DATA_LOG_SEG_NUM(x) \
      (((x)*sizeof(sl_data_log_data_t))/SL_BTMESH_DATA_LOG_LENGTH_MAX) \
      + (((x)*sizeof(sl_data_log_data_t))%SL_BTMESH_DATA_LOG_LENGTH_MAX? 1 : 0)

/// Determine last package of the data log will be sent
#define SL_BTMESH_DATA_LOG_LAST           1
#define SL_BTMESH_DATA_LOG_NOT_LAST       0

/// Data logging status
#define SL_BTMESH_DATA_LOG_IDLE           0
#define SL_BTMESH_DATA_LOG_BUSY           1
#define SL_BTMESH_DATA_LOG_COMPLETE       2

/// High Priority
#define HIGH_PRIORITY                  0
/// No Timer Options
#define NO_FLAGS                       0
/// Callback has not parameters
#define NO_CALLBACK_DATA               (void *)NULL

/// There're remaining segments
#define SL_BTMESH_SEGMENT_CONTI           0
/// The final segment
#define SL_BTMESH_SEGMENT_FINAL           1

/// Publish this model
#define SL_BTMESH_MODEL_PUBLISH           1

#define SL_BTMESH_DATA_LOG_CLEAR_VAL      0

/// Max Data length of the properties in byte
#define SL_BTMESH_DATA_LOG_PROP_LEN       4

/// Reset value
#define SL_BTMESH_DATA_LOG_RESET_VAL      (0)

/// Type of the Data Logging element index
typedef uint16_t sl_data_log_index_t;

/// Data structure for the data Logging
typedef struct sl_data_log {
  sl_data_log_index_t index;
  sl_data_log_data_t *data;
}sl_data_log_t;

/// Data structure for the data sending by stack
typedef struct sl_data_frame {
  uint8_t last;
  sl_data_log_data_t *data;
}sl_data_frame_t;

/// Data type for the Log sample rate
typedef uint32_t sl_btmesh_data_log_sample_rate_t;

/// Data type for the logging period
typedef uint32_t sl_btmesh_data_log_period_t;

/// Data type for the raw data
typedef int8_t sl_btmesh_data_log_raw_t;

/// Data structure for the Data Log properties
struct sl_btmesh_data_log_properties_s {
    sl_btmesh_data_log_sample_rate_t sample_rate;
    sl_btmesh_data_log_period_t period;
    sl_btmesh_data_log_raw_t raw_data;
};

typedef struct sl_btmesh_data_log_properties_s sl_btmesh_data_log_properties_t;

/// Message Opcodes
extern const uint8_t sl_btmesh_data_log_opcodes[];

#endif // SL_BTMESH_DATA_LOGGING_CAPI_H

#ifdef __cplusplus
}
#endif
