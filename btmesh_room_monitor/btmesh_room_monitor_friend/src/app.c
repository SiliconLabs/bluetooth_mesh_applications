/***************************************************************************//**
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <stdio.h>
#include "em_common.h"
#include "app_assert.h"
#include "sl_status.h"
#include "app.h"

#include "gatt_db.h"

#include "sl_btmesh_api.h"
#include "sl_bt_api.h"

#include "sl_btmesh_factory_reset.h"

#include "sl_simple_button_instances.h"
#include "sl_simple_button.h"

// -----------------------------------------------------------------------------
// Led
#if defined(SL_CATALOG_SIMPLE_LED_PRESENT) && defined(SL_CATALOG_LED0_PRESENT)
#include "sl_simple_led_instances.h"
#define led0_on()     sl_simple_led_turn_on(sl_led_led0.context);
#define led0_off()    sl_simple_led_turn_off(sl_led_led0.context);
#define led0_toggle() sl_simple_led_toggle(sl_led_led0.context);
#else
#define led0_on()
#define led0_off()
#define led0_toggle()
#endif

/// Advertising Provisioning Bearer
#define PB_ADV                         0x1
/// GATT Provisioning Bearer
#define PB_GATT                        0x2

/// periodic timer handle
static sl_sleeptimer_timer_handle_t app_led_blinking_timer;

static void start_led_blink(uint32_t period_ms);
static void stop_led_blink(void);
static void app_led_blinking_timer_cb(sl_sleeptimer_timer_handle_t *timer, void *data);
static void handle_boot_event(void);
static void handle_node_initialized_event(sl_btmesh_evt_node_initialized_t *evt);
static bool handle_reset_conditions(void);

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(struct sl_bt_msg *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_system_boot_id:
      handle_boot_event();
      break;

    // -------------------------------
    // Default event handler.
    default:
      break;
  }
}

/**************************************************************************//**
 * Bluetooth Mesh stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth Mesh stack.
 *****************************************************************************/
void sl_btmesh_on_event(sl_btmesh_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_initialized_id:
      handle_node_initialized_event(&(evt->data.evt_node_initialized));
      break;

    // -------------------------------
    // Default event handler.
    default:
      break;
  }
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
  char name[36];
  sl_status_t result;

  // Create unique device name using the last two bytes of the Bluetooth address
  snprintf(name,
           sizeof(name),
           "room monitor relay %02x:%02x",
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

    // Initialize Mesh stack in Node operation mode, wait for initialized event
    sc = sl_btmesh_node_init();
    app_assert_status_f(sc);
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
  if (!evt->provisioned) {
    // Enable ADV and GATT provisioning bearer
    sl_status_t sc = sl_btmesh_node_start_unprov_beaconing(PB_ADV | PB_GATT);

    app_assert_status_f(sc, "Failed to start unprovisioned beaconing\n");

    // blink led: 1 second on, 1 second off
    start_led_blink(1000);
  }
}

static void start_led_blink(uint32_t period_ms)
{
  sl_status_t sc;
  bool running;

  sc = sl_sleeptimer_is_timer_running(&app_led_blinking_timer, &running);
  app_assert_status(sc);
  if(running) {
    sc = sl_sleeptimer_restart_periodic_timer_ms( &app_led_blinking_timer,
                                                period_ms,
                                                app_led_blinking_timer_cb,
                                                NULL,
                                                0,
                                                0);
  }
  else {
    sc = sl_sleeptimer_start_periodic_timer_ms( &app_led_blinking_timer,
                                                period_ms,
                                                app_led_blinking_timer_cb,
                                                NULL,
                                                0,
                                                0);
  }
  app_assert_status_f(sc, "Failed to start periodic timer\r\n");
}

static void stop_led_blink(void)
{
  sl_status_t sc;
  bool running = 0;
  sc = sl_sleeptimer_is_timer_running(&app_led_blinking_timer, &running);
  app_assert_status(sc);
  if(running) {
    sc = sl_sleeptimer_stop_timer(&app_led_blinking_timer);
    app_assert_status_f(sc, "Failed to stop periodic timer\r\n");
    led0_off();
  }
}

/***************************************************************************//**
 * Handles button press and does a factory reset
 *
 * @return true if there is no button press
 ******************************************************************************/
static bool handle_reset_conditions(void)
{
  // If PB0 is held down then do full factory reset
  if (sl_simple_button_get_state(&sl_button_btn0)
      == SL_SIMPLE_BUTTON_PRESSED) {

    led0_on();

    // Full factory reset
    sl_btmesh_initiate_full_reset();
    return false;
  }
  return true;
}

/*******************************************************************************
 * Timer Callbacks
 ******************************************************************************/
/***************************************************************************//**
 * periodic timer callback
 * @param[in] handle Timer descriptor handle
 * @param[in] data Callback input arguments
 ******************************************************************************/
static void app_led_blinking_timer_cb(sl_sleeptimer_timer_handle_t *timer, void *data)
{
  (void)data;
  (void)timer;
  led0_toggle();
}

/*******************************************************************************
 * Callbacks
 ******************************************************************************/

/*******************************************************************************
 * Provisioning Decorator Callbacks
 ******************************************************************************/
/*******************************************************************************
 * Called when the Provisioning starts
 * @param[in] result Result code. 0: success, non-zero: error
 ******************************************************************************/
void sl_btmesh_on_node_provisioning_started(uint16_t result)
{
  app_log("BT mesh node provisioning is started (result: 0x%04x)\r\n", result);

  // blink led: 250 milliseconds on, 250 milliseconds off
  start_led_blink(250);
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
  sl_status_t sc = sl_sleeptimer_stop_timer(&app_led_blinking_timer);
  app_assert_status_f(sc, "Failed to stop periodic timer\r\n");

  // stop blink led
  stop_led_blink();

  // Turn off LED
  led0_off();

  app_log("BT mesh node is provisioned (address: 0x%04x, iv_index: 0x%lx)\r\n",
            address,
            iv_index);
}
