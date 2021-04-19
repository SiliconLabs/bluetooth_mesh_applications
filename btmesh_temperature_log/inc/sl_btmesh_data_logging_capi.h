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
*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef SL_BTMESH_DATA_LOGGING_CAPI_H
#define SL_BTMESH_DATA_LOGGING_CAPI_H

#include "sl_btmesh_api.h"
#include "sl_btmesh_data_logging_config.h"

/// Silabs company ID
#define SL_BTMESH_VENDOR_ID                     ((uint16_t)0x02FF)

/// Vendor model server ID
#define SL_BTMESH_DATA_LOG_MODEL_SERVER_ID      ((uint16_t)0x0000)

/// Vendor model client ID
#define SL_BTMESH_DATA_LOG_MODEL_CLIENT_ID      ((uint16_t)0x0001)

/// Opcode data length
#define SL_BTMESH_DATA_LOG_OPCODE_LENGTH        7

/// Data Log Status messages ID
#define SL_BTMESH_DATA_LOG_MESSAGE_STATUS_ID          ((uint8_t)0x01)
/// Data Log Status respond messages ID
#define SL_BTMESH_DATA_LOG_MESSAGE_STATUS_RSP_ID      ((uint8_t)0x02)
/// Data Log sending timing
#define SL_BTMESH_DATA_LOG_MESSAGE_PERIOD_ID          ((uint8_t)0x03)
/// Data Log sample timing
#define SL_BTMESH_DATA_LOG_MESSAGE_SAMPLE_RATE_ID     ((uint8_t)0x04)
/// Raw data used for specific purpose
#define SL_BTMESH_DATA_LOG_MESSAGE_THRESHOLD_ID       ((uint8_t)0x05)
/// Temperature data
#define SL_BTMESH_DATA_LOG_MESSAGE_TEMP_ID            ((uint8_t)0x06)
/// Temperature data request
#define SL_BTMESH_DATA_LOG_MESSAGE_TEMP_REQ_ID        ((uint8_t)0x07)

/// Client response cmd length
#define SL_BTMESH_DATA_LOG_MESSAGE_STATUS_RSP_LEN     ((uint8_t)0)
#define SL_BTMESH_DATA_LOG_MESSAGE_STATUS_RSP_DATA    NULL

/// Maximum data length of a vendor model massage (byte)
#define SL_BTMESH_STACK_SEND_LENGTH_MAX           ((uint16_t)247)

/// Determine last package of the data log will be sent
#define SL_BTMESH_DATA_LOG_LAST           ((uint8_t)1)
#define SL_BTMESH_DATA_LOG_NOT_LAST       ((uint8_t)0)

/// Data logging status
#define SL_BTMESH_DATA_LOG_IDLE           ((uint8_t)0)
#define SL_BTMESH_DATA_LOG_BUSY           ((uint8_t)1)
#define SL_BTMESH_DATA_LOG_COMPLETE       ((uint8_t)2)

/// High Priority
#define HIGH_PRIORITY                  ((uint8_t)0)
/// No Timer Options
#define NO_FLAGS                       ((uint16_t)0)
/// Callback has not parameters
#define NO_CALLBACK_DATA               (void *)NULL

/// There're remaining segments
#define SL_BTMESH_SEGMENT_CONTI           ((uint8_t)0)
/// The final segment
#define SL_BTMESH_SEGMENT_FINAL           ((uint8_t)1)

/// Publish this model
#define SL_BTMESH_MODEL_PUBLISH           ((uint8_t)1)

#define SL_BTMESH_DATA_LOG_CLEAR_VAL      0

/// Max Data length of the properties in byte
#define SL_BTMESH_DATA_LOG_PROP_LEN       4

/// Reset value
#define SL_BTMESH_DATA_LOG_RESET_VAL      ((uint16_t)0)

/// The "Last" flag in receive array
#define SL_BTMESH_BYTE_FLAG_POS     0
#define SL_BTMESH_BYTE_FLAG_LEN     1

/// Type of the Data Logging element index
typedef uint16_t sl_data_log_index_t;

/// Data structure for the data Logging
typedef struct sl_data_log {
  sl_data_log_index_t index;
  sl_data_log_data_t *data;
}sl_data_log_t;

/// Data structure for the data Logging receiving
typedef struct sl_data_log_recv {
  uint16_t source_addr;
  uint16_t dest_addr;
  sl_data_log_index_t index;
  sl_data_log_data_t *data;
}sl_data_log_recv_t;

/// Data structure header of the data sending by stack
typedef struct sl_data_frame_header {
  uint8_t last;
  uint8_t count;
}sl_data_frame_header_t;

/// Data structure of the data sending by stack
PACKSTRUCT(struct sl_data_frame {
  sl_data_frame_header_t header;
  sl_data_log_data_t *data;
});

typedef struct sl_data_frame sl_data_frame_t;

/// Data type for the Log sample rate
typedef uint32_t sl_btmesh_data_log_sample_rate_t;

/// Data type for the logging period
typedef uint32_t sl_btmesh_data_log_period_t;

/// Data type for the raw data
typedef int8_t sl_btmesh_data_log_threshold_t;

/// Data structure for the Data Log properties
struct sl_btmesh_data_log_properties_s {
    sl_btmesh_data_log_sample_rate_t sample_rate;
    sl_btmesh_data_log_period_t period;
    sl_btmesh_data_log_threshold_t threshold;
};
typedef struct sl_btmesh_data_log_properties_s sl_btmesh_data_log_properties_t;

/// Handshake information data length (byte)
#define  SL_BTMESH_DATA_LOG_INFO_LENGTH       sizeof(sl_data_frame_header_t)
/// Maximum length in one data sending
/// The max length is decreased because of the additional handshake information
#define SL_BTMESH_DATA_LOG_LENGTH_MAX (SL_BTMESH_STACK_SEND_LENGTH_MAX \
                                       - SL_BTMESH_DATA_LOG_INFO_LENGTH)

/// Number of data index sent in one publication
#define SL_BTMESH_DATA_LOG_SEG_NUM(x) \
      (((x)*sizeof(sl_data_log_data_t))/SL_BTMESH_DATA_LOG_LENGTH_MAX) \
      + (((x)*sizeof(sl_data_log_data_t))%SL_BTMESH_DATA_LOG_LENGTH_MAX? 1 : 0)

/// Frame header length
#define SL_BTMESH_DATA_HEADER_LEN     sizeof(sl_data_frame_header_t)

/// Message Opcodes
extern const uint8_t sl_btmesh_data_log_opcodes[];

#endif // SL_BTMESH_DATA_LOGGING_CAPI_H

#ifdef __cplusplus
}
#endif
