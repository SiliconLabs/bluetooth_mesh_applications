/***************************************************************************//**
* @file sl_btmesh_data_logging_capi.c
* @brief BT Mesh Data Logging Instances
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

#include "sl_btmesh_data_logging_capi.h"

/// Message Opcodes instance
const uint8_t sl_btmesh_data_log_opcodes[SL_BTMESH_DATA_LOG_OPCODE_LENGTH] = {
    SL_BTMESH_DATA_LOG_MESSAGE_STATUS_ID,
    SL_BTMESH_DATA_LOG_MESSAGE_STATUS_RSP_ID,
    SL_BTMESH_DATA_LOG_MESSAGE_PERIOD_ID,
    SL_BTMESH_DATA_LOG_MESSAGE_SAMPLE_RATE_ID,
    SL_BTMESH_DATA_LOG_MESSAGE_THRESHOLD_ID,
    SL_BTMESH_DATA_LOG_MESSAGE_TEMP_ID,
    SL_BTMESH_DATA_LOG_MESSAGE_TEMP_REQ_ID
};

#ifdef __cplusplus
}
#endif
