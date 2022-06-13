/***************************************************************************//**
 * @file vl53l1x_app.h
 * @brief Distance sampling and people counting module with VL53L1X Sensor
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef VL53L1X_APP_H
#define VL53L1X_APP_H

/**
 * VL53L1x callback event
 */
enum VL53L1X_APP_EVENT {
  VL53L1X_APP_EVENT_SOMEONE_ENTER,  //!< Event trigger when someone enter
  VL53L1X_APP_EVENT_SOMEONE_LEAVE   //!< Event trigger when someone leave
};

/***************************************************************************//**
 * @brief
 *    Init VL53L1x sensor for the people counting application.
 *
 * @param[in] min_distance
 *    Minimum distance
 *
 * @param[in] max_distance
 *    Maximum distance
 *
 * @param[in] distance_threshold
 *    Distance threshold
 *
 * @param[in] timing_budget
 *    Timing budget
 *
 ******************************************************************************/
void vl53l1x_app_init(uint16_t min_distance,
                     uint16_t max_distance,
                     uint16_t distance_threshold,
                     uint16_t timing_budget);

/***************************************************************************//**
 * @brief
 *    Get and process sampling data from the VL53L1x sensor.
 *
 ******************************************************************************/
void vl53l1x_app_process_sampling_data(void);

/**************************************************************************//**
 * @brief
 *   Get current measured distance.
 *
 * @return
 *   Current measured distance
*****************************************************************************/
uint16_t vl53l1x_app_get_current_measured_distance(void);

/**************************************************************************//**
 * @brief
 *   Get current number of people count.
 *
 * @return
 *   Current number of people count
*****************************************************************************/
uint16_t vl53l1x_app_get_people_count(void);

/**************************************************************************//**
 * @brief
 *   Change current timing budget.
 *
 * @param[in] timing_budget
 *   Current number of people count
*****************************************************************************/
void vl53l1x_app_change_timing_budget_in_ms(uint16_t timing_budget);

/**************************************************************************//**
 * @brief
 *   The callback when a event triggered: people enter, peole leave.
 *
 * @param[in] evt
 *   Event name
*****************************************************************************/
void vl53l1x_app_on_event(enum VL53L1X_APP_EVENT evt);

#endif // VL53L1X_APP_H
