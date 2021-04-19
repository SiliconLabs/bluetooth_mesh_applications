
/***************************************************************************//**
* @file sl_btmesh_temperature.c
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
#include "em_common.h"
#include "sl_app_assert.h"
#include "sl_app_log.h"
#include "sl_status.h"

#include "sl_btmesh_temperature.h"

#ifdef SL_BTMESH_TEMPERATURE_SI70XX_PRESENT
static sl_status_t sli_btmesh_temperature_si70xx_init(void);
static sl_status_t sli_btmesh_temperature_si70xx_get_rht(temperature_8_t *temp,
                                                  percentage_8_t *rh);
#endif
/***************************************************************************//**
 * Initialize temperature sensor.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_temperature_init(void)
{
#ifdef SL_BTMESH_TEMPERATURE_SI70XX_PRESENT
  return sli_btmesh_temperature_si70xx_init();
#else
  return SL_STATUS_OK;
#endif
}

/***************************************************************************//**
 * Get temperature and relative humidity value.
 *
 * @param[out] temp Pointer to temperature value.
 * @param[out] rh Pointer to relative humidity value in percentage.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_temperature_get_rht(temperature_8_t *temp,
                                          percentage_8_t *rh)
{
#ifdef SL_BTMESH_TEMPERATURE_SI70XX_PRESENT
  return sli_btmesh_temperature_si70xx_get_rht(temp, rh);
#else
  // Simulated values
  *temp = 25;
  *rh = 50;
  return SL_STATUS_OK;
#endif
}


#ifdef SL_BTMESH_TEMPERATURE_SI70XX_PRESENT
/***************************************************************************//**
 * Initialize si70xx temperature sensor.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
static sl_status_t sli_btmesh_temperature_si70xx_init(void)
{
  sl_status_t sc;
  uint8_t device_id;
  // Init temperature/humid sensor
  sc = sl_si70xx_init(SL_BTMESH_I2CSPM_INST,
                      SL_BTMESH_TEMPERATURE_SI70XX_ADDR);
  if(!sl_si70xx_present(SL_BTMESH_I2CSPM_INST, SL_BTMESH_I2CSPM_ADDR, &device_id)){
      sl_app_log("Temp sensor is not available");
  }

  return sc;
}
#endif //SL_BTMESH_TEMPERATURE_SI70XX_PRESENT


#ifdef SL_BTMESH_TEMPERATURE_SI70XX_PRESENT
/***************************************************************************//**
 * Get temperature and relative humidity from si70xx temperature sensor.
 *
 * @param[out] temp Pointer to temperature value.
 * @param[out] rh Pointer to relative humidity value in percentage.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
static sl_status_t sli_btmesh_temperature_si70xx_get_rht(temperature_8_t *temp,
                                                  percentage_8_t *rh)
{
  sl_status_t sc;
  uint32_t rhData;
  int32_t tData;
  // Get temperature
  sc = sl_si70xx_measure_rh_and_temp(SL_BTMESH_I2CSPM_INST,
                                     SL_BTMESH_I2CSPM_ADDR,
                                     &rhData,
                                     &tData);
  sl_app_assert(sc != SL_STATUS_FAIL,
                "[E: 0x%04x] Failed to get temperature\n",
                (int)sc);

  // Convert to normal temperature scale
  tData = tData/TEMPERATURE_SCALE_VAL;
  *temp = (temperature_8_t)tData;

  rhData = rhData/RHUMID_SCALE_VAL;
  *rh = (percentage_8_t)rhData;

  return sc;
}
#endif //SL_BTMESH_TEMPERATURE_SI70XX_PRESENT

#ifdef __cplusplus
}
#endif
