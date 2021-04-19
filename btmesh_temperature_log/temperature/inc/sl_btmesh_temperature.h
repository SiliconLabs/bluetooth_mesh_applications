/***************************************************************************//**
* @file sl_btmesh_temperature.h
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

#ifndef SL_BTMESH_TEMPERATURE_H
#define SL_BTMESH_TEMPERATURE_H
#include "sl_btmesh_device_properties.h"
#include "sl_btmesh_temperature_config.h"
#ifdef SL_BTMESH_TEMPERATURE_SI70XX_PRESENT
#include "sl_i2cspm_instances.h"
#include "sl_si70xx.h"
#endif

#ifdef SL_I2CSPM_TEMPERATURE_ENV_PRESENT
#define SL_BTMESH_I2CSPM_INST     sl_i2cspm_temperature_env
#define SL_BTMESH_I2CSPM_ADDR     SI7021_ADDR
#endif

/// Initialize temperature sensor
sl_status_t sl_btmesh_temperature_init(void);

/// Get temperature and relative humidity value
sl_status_t sl_btmesh_temperature_get_rht(temperature_8_t *temp,
                                          percentage_8_t *rh);
#endif // SL_BTMESH_TEMPERATURE_H

#ifdef __cplusplus
}
#endif
