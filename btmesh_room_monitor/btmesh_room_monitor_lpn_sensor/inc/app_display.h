/***************************************************************************//**
 * @file app_display.h
 * @brief Header file of app_display.c
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
#ifndef APP_DISPLAY_H
#define APP_DISPLAY_H

#include "stdint.h"
#include "sl_bgapi.h"
#include "sl_simple_timer.h"

/***************************************************************************//**
 * @addtogroup app_display
 * @brief app_display interface.
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *    Typedef for holding display parameters.
 ******************************************************************************/
typedef struct {
  uint8_t param_text[32]; ///< Threshold parameter text
  uint8_t *first_letter; ///<  Actual first letter of the param_text
  uint8_t max_char_per_line; ///< Maximum length of characters to be displayed
  uint8_t text_length; ///< Length of the param_text
  sl_simple_timer_t timer_handle; ///< Timer handler for screen update
} unprovisioned_display_data_t;

/**************************************************************************//**
 * @brief
 *  Initialize the app_display application.
 *
 *****************************************************************************/
void app_display_init(void);

/***************************************************************************//**
 * @brief
 *  Show the 2-last byte of the bluetooth address on the screen
 *
 ******************************************************************************/
void app_display_set_name(bd_addr addr);

/***************************************************************************//**
 * @brief
 *   Show bluetooth mesh friendship connection status on screen.
 ******************************************************************************/
void app_display_show_friend_status(bool status);

/***************************************************************************//**
 * @brief
 *   Draw screen when device is unprovisioned.
 ******************************************************************************/
void app_display_show_unprovisioned(void);

/***************************************************************************//**
 * @brief
 *   Draw screen when device is factory reset.
 ******************************************************************************/
void app_display_show_factory_reset(void);

/***************************************************************************//**
 * @brief
 *   Draw screen when device is provisioned.
 ******************************************************************************/
void app_display_show_people_count(uint32_t people_count);

/** @} */

#endif  // APP_DISPLAY_H
