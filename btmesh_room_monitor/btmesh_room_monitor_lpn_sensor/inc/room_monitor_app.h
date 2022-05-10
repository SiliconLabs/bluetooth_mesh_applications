/***************************************************************************//**
 * @file room_monitor_app.h
 * @brief People counting application code
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

#ifndef ROOM_MONITOR_APP_H
#define ROOM_MONITOR_APP_H

/**************************************************************************//**
 * @brief
 *   Initialization function for the room monitor module.
 *
*****************************************************************************/
void room_monitor_app_init(void);

/***************************************************************************//**
 * @brief
 *    Start sampling timer.
 *
 * @param[in] delay_ms
 *    Delay time before start sampling
 *
 ******************************************************************************/
void room_monitor_app_start_sampling(uint16_t delay_ms);

/***************************************************************************//**
 * @brief
 *    Stop sampling timer.
 *
 ******************************************************************************/
void room_monitor_app_stop_sampling(void);

/***************************************************************************//**
 * @brief
 *    Handle bluetooth event external signal trigger by room monitor module.
 *
 * @param[in] extsignals
 *    Event flags
 *
 ******************************************************************************/
void people_counting_process_evt_external_signal(uint32_t extsignals);

#endif // ROOM_MONITOR_APP_H
