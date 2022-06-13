/***************************************************************************//**
 * @file room_monitor_app.c
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
#include <stdio.h>
#include "em_common.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#ifdef SL_CATALOG_SIMPLE_LED_PRESENT
#include "sl_simple_led_instances.h"
#endif
#include "sl_simple_button_instances.h"

#include "sl_btmesh_sensor_people_count.h"
#include "sl_btmesh_sensor_people_count_config.h"

#include "app_display.h"
#include "app.h"
#include "vl53l1x_app.h"
#include "room_monitor_app.h"

// -----------------------------------------------------------------------------
// Logging
#define TAG "room monitor"
// use applog for the log printing
#if defined(SL_CATALOG_APP_LOG_PRESENT) && APP_LOG_ENABLE
#include "app_log.h"
#define log_info(fmt, ...)  app_log_info("[" TAG "] " fmt, ##__VA_ARGS__)
#define log_error(fmt, ...) app_log_error("[" TAG "] " fmt, ##__VA_ARGS__)
// use stdio printf for the log printing
#elif defined(SL_CATALOG_RETARGET_STDIO_PRESENT)
#define log_info(fmt, ...)   printf("[" TAG "] " fmt, ##__VA_ARGS__)
#define log_error(fmt, ...)  printf("[" TAG "] " fmt, ##__VA_ARGS__)
#else  // the logging is disabled
#define log_info(...)
#define log_error(...)
#endif // #if defined(SL_CATALOG_APP_LOG_PRESENT)

// -----------------------------------------------------------------------------
// Led
#if defined(SL_CATALOG_SIMPLE_LED_PRESENT) && defined(SL_CATALOG_LED0_PRESENT)
#include "sl_simple_led_instances.h"
#define led0_on()   sl_led_turn_on(&sl_led_led0);
#define led0_off()  sl_led_turn_off(&sl_led_led0);
#else
#define led0_on()
#define led0_off()
#endif

/***************************************************************************//**
 * @addtogroup room_monitor_app
 * @brief  People counting application.
 * @details
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Defines

#define SAMPLING_PERIOD_MS              (2)
#define MIN_DISTANCE                    (0)
#define MAX_DISTANCE                    (2700)
#define DISTANCE_THRESHOLD              (1600)
#define TIMING_BUDGET                   (33)

#define PEOPLE_COUNTING_BUTTON_EVENT    (1<<0)
#define PEOPLE_COUNTING_SAMPLING_EVENT  (1<<1)
#define PEOPLE_COUNTING_EVENT           (1<<2)

// -----------------------------------------------------------------------------
// Private variables
static uint32_t people_entered_so_far;

static sl_sleeptimer_timer_handle_t oled_timer;
static sl_sleeptimer_timer_handle_t people_counting_timer;

static uint16_t startup_delay_ms = 0;

// -----------------------------------------------------------------------------
// Private function declarations

static void people_counting_button_handler(void);
static void people_counting_event_handler(void);
static void people_counting_oled_display_callback(
    sl_sleeptimer_timer_handle_t *timer, void *data);
static void people_counting_sensor_sampling_callback(
    sl_sleeptimer_timer_handle_t *timer, void *data);


// -----------------------------------------------------------------------------
// Public function definitions

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
void room_monitor_app_init(void)
{
  people_entered_so_far = 0;

  vl53l1x_app_init(MIN_DISTANCE,
                  MAX_DISTANCE,
                  DISTANCE_THRESHOLD,
                  TIMING_BUDGET);

  log_info("OLED initialized\r\n");
  app_display_init();

  sl_btmesh_set_people_count(0);
}

/**************************************************************************//**
 * Start Sampling.
 *****************************************************************************/
void room_monitor_app_start_sampling(uint16_t delay_ms)
{
  bool running;
  sl_status_t sc;

  startup_delay_ms = delay_ms;
  if(!delay_ms) {
    delay_ms = 1;
  }
  sc = sl_sleeptimer_is_timer_running(&people_counting_timer, &running);
  app_assert_status(sc);
  // Create sampling and calculate people count periodic timer
  if(!running) {
    sc = sl_sleeptimer_start_periodic_timer_ms( &people_counting_timer,
                                                delay_ms,
                                                people_counting_sensor_sampling_callback,
                                                NULL,
                                                0,
                                                0);
    app_assert_status(sc);
  }

  sc = sl_sleeptimer_is_timer_running(&oled_timer, &running);
  app_assert_status(sc);


  // Create oled display periodic timer
  if(!running) {
    sl_sleeptimer_start_periodic_timer_ms(&oled_timer,
                                          1000,
                                          people_counting_oled_display_callback,
                                          NULL,
                                          0,
                                          0);
  }
}

/**************************************************************************//**
 * Stop Sampling.
 *****************************************************************************/
void room_monitor_app_stop_sampling(void)
{
  bool running;
  sl_status_t sc;

  sc = sl_sleeptimer_is_timer_running(&people_counting_timer, &running);
  app_assert_status(sc);
  if(running) {
    sc = sl_sleeptimer_stop_timer(&people_counting_timer);
    app_assert_status(sc);
  }
}

/**************************************************************************//**
 * People Counting Application Process External Signal.
 *****************************************************************************/
void people_counting_process_evt_external_signal(uint32_t extsignals)
{
  if (extsignals & PEOPLE_COUNTING_BUTTON_EVENT) {
    people_counting_button_handler();
  }

  if (extsignals & PEOPLE_COUNTING_SAMPLING_EVENT) {
    if(startup_delay_ms) {
      startup_delay_ms = 0;
      // Create sampling and calculate people count periodic timer
      sl_sleeptimer_restart_periodic_timer_ms(&people_counting_timer,
                                            SAMPLING_PERIOD_MS,
                                            people_counting_sensor_sampling_callback,
                                            NULL,
                                            0,
                                            0);
    }
    else {
      vl53l1x_app_process_sampling_data();
    }
  }

  if (extsignals & PEOPLE_COUNTING_EVENT) {
    people_counting_event_handler();
  }
}

// -----------------------------------------------------------------------------
// Callback function

void vl53l1x_app_on_event(enum VL53L1X_APP_EVENT evt)
{
  switch(evt) {
    case VL53L1X_APP_EVENT_SOMEONE_ENTER:
      sl_btmesh_people_count_increase();
      people_entered_so_far++;
      log_info("Someone In, People Count=%d\r\n", sl_btmesh_get_people_count());
      break;
    case VL53L1X_APP_EVENT_SOMEONE_LEAVE:
      sl_btmesh_people_count_decrease();
      log_info("Someone Out, People Count=%d\r\n", sl_btmesh_get_people_count());
      break;
  }
}

// -----------------------------------------------------------------------------
// Private function

static void people_counting_button_handler(void)
{

}

static void people_counting_event_handler(void)
{
  static uint16_t last_people_count = (uint16_t)-1;
  uint16_t current_people_count = sl_btmesh_get_people_count();

  // Only display & notify people count when their value is changed
  if ( last_people_count != current_people_count) {
    last_people_count = current_people_count;

    // Display people count on oled screen
    app_display_show_people_count(current_people_count);
  }
}

/***************************************************************************//**
 * Callback on timer period.
 ******************************************************************************/
static void people_counting_sensor_sampling_callback(
    sl_sleeptimer_timer_handle_t *timer, void *data)
{
  (void) timer;
  (void) data;
  sl_bt_external_signal(PEOPLE_COUNTING_SAMPLING_EVENT);
}

/***************************************************************************//**
 * Callback on timer period.
 ******************************************************************************/
static void people_counting_oled_display_callback(
    sl_sleeptimer_timer_handle_t *timer, void *data)
{
  (void) timer;
  (void) data;
  sl_bt_external_signal(PEOPLE_COUNTING_EVENT);
}

/** @} (end group room_monitor_app) */

