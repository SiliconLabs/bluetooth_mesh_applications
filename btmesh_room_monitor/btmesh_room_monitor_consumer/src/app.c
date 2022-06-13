/***************************************************************************//**
 * @file
 * @brief Silicon Labs SoC Sensor Server Example Project
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
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
 ******************************************************************************/
#include <stdbool.h>
#include <stdio.h>
#include "em_common.h"
#include "sl_status.h"

#include "sl_btmesh.h"
#include "sl_bluetooth.h"
#include "app.h"

#include "gatt_db.h"

#include "app_log.h"
#include "app_assert.h"

#include "sl_simple_button.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led.h"
#include "sl_simple_led_instances.h"
#include "sl_simple_timer.h"

#include "app_display.h"
#include "app_sensor_client.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_BTMESH_WSTK_LCD_PRESENT
#include "sl_btmesh_wstk_lcd.h"
#endif // SL_CATALOG_BTMESH_WSTK_LCD_PRESENT

#include "sl_btmesh_factory_reset.h"

// High Priority
#define HIGH_PRIORITY                  0
// No Timer Options
#define NO_FLAGS                       0
// Callback has not parameters
#define NO_CALLBACK_DATA               (void *)NULL
// timeout for periodic sensor data update
#define SENSOR_DATA_TIMEOUT            3000
// Timout for Blinking LED during provisioning
#define APP_LED_BLINKING_TIMEOUT       250
// Connection uninitialized
#define UNINITIALIZED_CONNECTION       0xFF
// Advertising Provisioning Bearer
#define PB_ADV                         0x1
// GATT Provisioning Bearer
#define PB_GATT                        0x2
// Length of the display name buffer
#define NAME_BUF_LEN                   20
// Length of boot error message buffer
#define BOOT_ERR_MSG_BUF_LEN           30

// Number of active Bluetooth connections
static uint8_t num_connections = 0;

static bool init_done = false;

static mesh_device_properties_t current_property = PEOPLE_COUNT;

// periodic timer handle
static sl_simple_timer_t app_led_blinking_timer;
static sl_simple_timer_t app_sensor_data_timer;

// periodic timer callback
static void app_led_blinking_timer_cb(sl_simple_timer_t *handle, void *data);
static void app_sensor_data_timer_cb(sl_simple_timer_t *handle, void *data);
// Handling of boot event
static void handle_boot_event(void);
// Handling of mesh node initialized event
static void handle_node_initialized_event(sl_btmesh_evt_node_initialized_t *evt);
// Handling of mesh node provisioning event
static void handle_node_provisioning_events(sl_btmesh_msg_t *evt);
// Handling of mesh node event
static void sl_btmesh_handle_app_on_event(sl_btmesh_msg_t *evt);
// Set device name in the GATT database
static void set_device_name(bd_addr *addr);

/***************************************************************************//**
 * Application Init.
 ******************************************************************************/
SL_WEAK void app_init(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
  app_log("BT mesh Consumer initialized\r\n");
  app_log("OLED initialized\r\n");
  app_display_init();
}

/***************************************************************************//**
 * Application Process Action.
 ******************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

/*******************************************************************************
 * Get the currently set property ID
 *
 * @return Current property ID
 ******************************************************************************/
mesh_device_properties_t app_get_current_property(void)
{
  return current_property;
}

/***************************************************************************//**
 * Set device name in the GATT database. A unique name is generated using
 * the two last bytes from the Bluetooth address of this device. Name is also
 * displayed on the LCD.
 *
 * @param[in] addr  Pointer to Bluetooth address.
 ******************************************************************************/
static void set_device_name(bd_addr *addr)
{
  char name[NAME_BUF_LEN];
  sl_status_t result;

  // Create unique device name using the last two bytes of the Bluetooth address
  snprintf(name,
           sizeof(name),
           "mrm consumer %02x:%02x",
           addr->addr[1],
           addr->addr[0]);

  app_log("Device name: '%s'\r\n", name);

  result = sl_bt_gatt_server_write_attribute_value(gattdb_device_name,
                                                   0,
                                                   strlen(name),
                                                   (uint8_t *)name);
  if (result) {
    app_log("sl_bt_gatt_server_write_attribute_value() failed, code %x\r\n",
            result);
  }
}

/***************************************************************************//**
 * Handles button press and does a factory reset
 *
 * @return true if there is no button press
 ******************************************************************************/
bool handle_reset_conditions(void)
{
  // If PB0 is held down then do full factory reset
  if (sl_simple_button_get_state(&sl_button_btn0)
      == SL_SIMPLE_BUTTON_PRESSED) {
    // Show log to oled
    app_display_show_factory_reset();
    // Full factory reset
    sl_btmesh_initiate_full_reset();
    return false;
  }
  return true;
}

/***************************************************************************//**
 * Handling of boot event.
 * If needed it performs factory reset. In other case it sets device name
 * and initialize mesh node.
 ******************************************************************************/
static void handle_boot_event(void)
{
  sl_status_t sc;
  bd_addr address;
  uint8_t address_type;

  // Check reset conditions and continue if not reset.
  if (handle_reset_conditions()) {
    sc = sl_bt_system_get_identity_address(&address, &address_type);
    app_assert_status_f(sc, "Failed to get Bluetooth address\r\n");
    set_device_name(&address);
    app_display_set_name(address);
    // Initialize Mesh stack in Node operation mode, wait for initialized event
    sc = sl_btmesh_node_init();
    if (sc != SL_STATUS_OK) {
      app_log("init failed (0x%lx)\n\r", sc);
    }
  }
}

/***************************************************************************//**
 * Handling of mesh node initialized event.
 * If device is provisioned it initializes the sensor server node.
 * If device is unprovisioned it starts sending Unprovisioned Device Beacons.
 *
 * @param[in] evt  Pointer to mesh node initialized event.
 ******************************************************************************/
static void handle_node_initialized_event(sl_btmesh_evt_node_initialized_t *evt)
{
  if (evt->provisioned) {
    sl_status_t sc =
      sl_simple_timer_start(&app_sensor_data_timer,
                            SENSOR_DATA_TIMEOUT,
                            app_sensor_data_timer_cb,
                            NO_CALLBACK_DATA,
                            true);
    app_assert_status_f(sc, "Failed to start timer\r\n");
  } else {
    // Enable ADV and GATT provisioning bearer
    sl_status_t sc = sl_btmesh_node_start_unprov_beaconing(PB_ADV | PB_GATT);

    app_assert_status_f(sc, "Failed to start unprovisioned beaconing\n");
  }
}

/***************************************************************************//**
 *  Handling of mesh node provisioning events.
 *  It handles:
 *   - mesh_node_provisioning_started
 *   - mesh_node_provisioned
 *   - mesh_node_provisioning_failed
 *
 *  @param[in] evt  Pointer to incoming provisioning event.
 ******************************************************************************/
static void handle_node_provisioning_events(sl_btmesh_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    sl_status_t sc;
    case sl_btmesh_evt_node_provisioned_id:
      // Start a periodic timer for get sensor data after startup
      sc = sl_simple_timer_start(&app_sensor_data_timer,
                                 SENSOR_DATA_TIMEOUT,
                                 app_sensor_data_timer_cb,
                                 NO_CALLBACK_DATA,
                                 true);
      app_assert_status_f(sc, "Failed to start timer\r\n");
      break;

    default:
      break;
  }
}

/***************************************************************************//**
 *  Handling of le connection events.
 *  It handles:
 *   - le_connection_opened
 *   - le_connection_parameters
 *   - le_connection_closed
 *
 *  @param[in] evt  Pointer to incoming connection event.
 ******************************************************************************/
static void handle_le_connection_events(sl_bt_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_connection_opened_id:
      num_connections++;
      app_log("Connected\r\n");
      break;

    case sl_bt_evt_connection_closed_id:
      if (num_connections > 0) {
        if (--num_connections == 0) {
          app_log("Disconnected\r\n");
        }
      }
      break;

    default:
      break;
  }
}

/***************************************************************************//**
 * Handling of stack events. Both Bluetooth LE and Bluetooth mesh events
 * are handled here.
 * @param[in] evt    Pointer to incoming event.
 ******************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_system_boot_id:
      handle_boot_event();
      break;

    case sl_bt_evt_connection_opened_id:
    case sl_bt_evt_connection_parameters_id:
    case sl_bt_evt_connection_closed_id:
      handle_le_connection_events(evt);
      break;

    default:
      break;
  }
}

/***************************************************************************//**
 * Bluetooth Mesh stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Pointer to incoming event from the Bluetooth Mesh stack.
 ******************************************************************************/
void sl_btmesh_on_event(sl_btmesh_msg_t *evt)
{
  sl_btmesh_handle_app_on_event(evt);
  sl_btmesh_handle_sensor_client_on_event(evt);
}

/***************************************************************************//**
 * Bluetooth Mesh app event handler.
 *
 * @param[in] evt Pointer to incoming event from the Bluetooth Mesh stack.
 ******************************************************************************/
static void sl_btmesh_handle_app_on_event(sl_btmesh_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_initialized_id:
      handle_node_initialized_event(&(evt->data.evt_node_initialized));
      break;

    case sl_btmesh_evt_node_provisioned_id:
      handle_node_provisioning_events(evt);
      break;

    default:
      break;
  }
}

/***************************************************************************//**
 * Timer Callbacks
 ******************************************************************************/
static void app_sensor_data_timer_cb(sl_simple_timer_t *handle,
                                     void *data)
{
  (void)data;
  (void)handle;
  app_log("Sent request data\r\n");
  sl_btmesh_sensor_client_get_sensor_data(current_property);
}

/*******************************************************************************
 * Timer Callbacks
 ******************************************************************************/
/***************************************************************************//**
 * periodic timer callback
 * @param[in] handle Timer descriptor handle
 * @param[in] data Callback input arguments
 ******************************************************************************/
static void app_led_blinking_timer_cb(sl_simple_timer_t *handle, void *data)
{
  (void)data;
  (void)handle;
  if (!init_done) {
    // Toggle LEDs
    sl_simple_led_toggle(sl_led_led0.context);
  }
}

/*******************************************************************************
 * Provisioning Decorator Callbacks
 ******************************************************************************/
/*******************************************************************************
 * Called when the Provisioning starts
 * @param[in] result Result code. 0: success, non-zero: error
 ******************************************************************************/
void sl_btmesh_on_node_provisioning_started(uint16_t result)
{
  sl_status_t sc = sl_simple_timer_start(&app_led_blinking_timer,
                                         APP_LED_BLINKING_TIMEOUT,
                                         app_led_blinking_timer_cb,
                                         NO_CALLBACK_DATA,
                                         true);
  app_assert_status_f(sc, "Failed to start periodic timer\r\n");

  app_show_btmesh_node_provisioning_started(result);
}

/*******************************************************************************
 * Called when the Provisioning finishes successfully
 * @param[in] address      Unicast address of the primary element of the node.
 *                         Ignored if unprovisioned.
 * @param[in] iv_index     IV index for the first network of the node
 *                         Ignored if unprovisioned.
 ******************************************************************************/
void sl_btmesh_on_node_provisioned(uint16_t address, uint32_t iv_index)
{
  sl_status_t sc = sl_simple_timer_stop(&app_led_blinking_timer);
  app_assert_status_f(sc, "Failed to stop periodic timer\r\n");
  // Turn off LED
  init_done = true;
  sl_simple_led_turn_off(sl_led_led0.context);

  app_show_btmesh_node_provisioned(address, iv_index);
}
