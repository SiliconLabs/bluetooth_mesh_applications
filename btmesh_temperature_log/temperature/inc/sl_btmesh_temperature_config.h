/***************************************************************************//**
* @file sl_btmesh_temperature_config.h
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

#ifndef SL_BTMESH_TEMPERATURE_CONFIG_H
#define SL_BTMESH_TEMPERATURE_CONFIG_H

// Use si70xx series temperature sensor
//#define SL_BTMESH_TEMPERATURE_SI70XX_PRESENT

/// SI70xx address
#define SL_BTMESH_TEMPERATURE_SI70XX_ADDR   SI7021_ADDR

/// Pre-scale value for temperature sensor raw data
#define TEMPERATURE_PRE_SCALE   ((uint32_t)2)
/// Offset value for temperature sensor pre-scaled value
#define TEMPERATURE_OFFSET      ((uint32_t)499)
/// Scale value for temperature sensor final value
#define TEMPERATURE_SCALE_VAL   ((uint32_t)1000)

/// Scale value for humidity sensor value
#define RHUMID_SCALE_VAL   ((uint32_t)1000)

#endif // SL_BTMESH_TEMPERATURE_CONFIG_H

#ifdef __cplusplus
}
#endif
