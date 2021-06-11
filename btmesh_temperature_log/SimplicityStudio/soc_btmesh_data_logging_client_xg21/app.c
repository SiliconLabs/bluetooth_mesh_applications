/***************************************************************************//**
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include "em_common.h"
#include "sl_app_assert.h"
#include "sl_app_log.h"
#include "sl_status.h"
#include "app.h"

#include "gatt_db.h"

#include "sl_btmesh_api.h"
#include "sl_bt_api.h"

#include "sl_btmesh_factory_reset.h"
/* Buttons and LEDs headers */
#include "sl_btmesh_button_press.h"
#include "sl_simple_button.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led.h"
#include "sl_simple_led_instances.h"
#include "sl_simple_timer.h"

#include "sl_btmesh_data_logging_client.h"

/// Advertising Provisioning Bearer
#define PB_ADV                         0x1
/// GATT Provisioning Bearer
#define PB_GATT                        0x2

#define LOG_SAMPLE_RATE                1500
#define LOG_PERIOD                     5000

/// Length of the display name buffer
#define NAME_BUF_LEN                   20
/// Timout for Blinking LED during provisioning
#define APP_LED_BLINKING_TIMEOUT       250

/// Buffer for the Log received
sl_data_log_data_t log_data_arr[SL_BTMESH_DATA_LOG_BUFF_SIZE_CFG_VAL];

sl_data_log_recv_t log_data = {
    .source_addr = 0,
    .dest_addr = 0,
    .index = 0,
    .data = log_data_arr
};

// Log received indication
bool log_received_flag = false;

/// periodic timer handle
static sl_simple_timer_t app_led_blinking_timer;

/// periodic timer callback
static void app_led_blinking_timer_cb(sl_simple_timer_t *handle, void *data);
/// Set device name in the GATT database
static void set_device_name(bd_addr *addr);

static bool init_done = false;

/***************************************************************************//**
 * Change buttons to LEDs in case of shared pin
 *
 ******************************************************************************/
void change_buttons_to_leds(void)
{
  sl_btmesh_button_press_disable();
  // Disable button and enable led
  sl_simple_button_disable(sl_button_btn0.context);
  sl_simple_led_init(sl_led_led0.context);
  // Disable button and enable led
#ifndef SINGLE_BUTTON
  sl_simple_button_disable(sl_button_btn1.context);
#endif // SINGLE_BUTTON
#ifndef SINGLE_LED
  sl_simple_led_init(sl_led_led1.context);
#endif //SINGLE_LED
}

/***************************************************************************//**
 * Change LEDs to buttons in case of shared pin
 *
 ******************************************************************************/
void change_leds_to_buttons(void)
{
  // Enable buttons
  sl_simple_button_enable(sl_button_btn0.context);
#ifndef SINGLE_BUTTON
  sl_simple_button_enable(sl_button_btn1.context);
#endif // SINGLE_BUTTON
  // Wait
  sl_sleeptimer_delay_millisecond(1);
  // Enable button presses
  sl_btmesh_button_press_enable();
}

/***************************************************************************//**
 * Print the received log
 *********************0********************************************************/
void print_log(void);

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
  sl_app_log("BT mesh Data Log Client initialized\r\n");
  // Ensure right init order in case of shared pin for enabling buttons
  change_buttons_to_leds();
  // Change LEDs to buttons in case of shared pin
  change_leds_to_buttons();
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
  sl_status_t sc;
  if(log_received_flag){
      print_log();
      sc = sl_btmesh_data_log_client_reset_log();
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to reset log\n",
                    (int)sc);
      log_received_flag = false;
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
  char name[NAME_BUF_LEN];
  sl_status_t result;

  // Create unique device name using the last two bytes of the Bluetooth address
  snprintf(name,
           sizeof(name),
           "LC %02x:%02x",
           addr->addr[1],
           addr->addr[0]);

  sl_app_log("Device name: '%s'\r\n", name);

  result = sl_bt_gatt_server_write_attribute_value(gattdb_device_name,
                                                   0,
                                                   strlen(name),
                                                   (uint8_t *)name);
  if (result) {
    sl_app_log("sl_bt_gatt_server_write_attribute_value() failed, code %x\r\n",
               result);
  }
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(struct sl_bt_msg *evt)
{
  sl_status_t sc;
  bd_addr address;
  uint8_t address_type;
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_system_boot_id:
      sc = sl_bt_system_get_identity_address(&address, &address_type);
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to get Bluetooth address\r\n",
                    (int)sc);
      set_device_name(&address);
      // Initialize Mesh stack in Node operation mode,
      // wait for initialized event
      sl_app_log("Node init\r\n");
      sc = sl_btmesh_node_init();
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to init node\n",
                    (int)sc);
      break;
    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////

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
  sl_status_t sc;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_initialized_id:
      sc = sl_btmesh_data_log_client_init(&log_data);
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to Init Log\n",
                    (int)sc);
      if (!evt->data.evt_node_initialized.provisioned) {
        // The Node is now initialized,
        // start unprovisioned Beaconing using PB-ADV and PB-GATT Bearers
        sl_app_log("Initialized\r\n");

        sc = sl_btmesh_node_start_unprov_beaconing(PB_ADV | PB_GATT);
        sl_app_assert(sc == SL_STATUS_OK,
                      "[E: 0x%04x] Failed to start unprovisioned beaconing\n",
                      (int)sc);
      }
      break;
    case sl_btmesh_evt_vendor_model_receive_id:
      sc = sl_btmesh_data_log_on_client_receive_event(evt);
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to handle event!\n",
                    (int)sc);
      break;
    case sl_btmesh_evt_node_provisioned_id:
      sl_app_log("Log Node provisioned\r\n");
      break;
    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////
    case sl_btmesh_evt_node_reset_id:
      sl_btmesh_initiate_factory_reset();
      break;
    // -------------------------------
    // Default event handler.
    default:
      break;
  }
}

/***************************************************************************//**
 * Log receive completed Callbacks
 ******************************************************************************/
void sl_btmesh_data_log_client_recv_complete_callback(void)
{
  log_received_flag = true;
  sl_app_log("Log received complete\r\n");
}

/***************************************************************************//**
 * Button press Callbacks
 ******************************************************************************/
void sl_btmesh_button_press_cb(uint8_t button, uint8_t duration)
{
  sl_status_t sc;
  uint32_t sample = LOG_SAMPLE_RATE;
  uint32_t period = LOG_PERIOD;
  (void)duration;
  if (button == SL_BTMESH_BUTTON_PRESS_BUTTON_0) {
    sl_app_log("Set sample rate\r\n");
    sc = sl_btmesh_data_log_client_set_sample_rate(&sample);
    sl_app_assert(sc == SL_STATUS_OK,
                  "[E: 0x%04x] Failed to set sample rate\n",
                  (int)sc);
   }
  else if(button == SL_BTMESH_BUTTON_PRESS_BUTTON_1){
      sl_app_log("Set period\r\n");
      sc = sl_btmesh_data_log_client_set_period(&period);
      sl_app_assert(sc == SL_STATUS_OK,
                  "[E: 0x%04x] Failed to set period\n",
                  (int)sc);
  }
}

/***************************************************************************//**
 * Print the received log
 ******************************************************************************/
void print_log(void)
{
  uint16_t count;
  sl_app_log("Received Len: %d\r\n", log_data.index);
  sl_app_log("Received Log: [ ");
  for(count = 0; count < log_data.index; count++){
      sl_app_log("%d ", log_data.data[count]);
  }
  sl_app_log("]\r\n");
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
#ifndef SINGLE_LED
    sl_simple_led_toggle(sl_led_led1.context);
#endif // SINGLE_LED
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
  (void)result;
  // Change buttons to LEDs in case of shared pin
  change_buttons_to_leds();

  sl_status_t sc = sl_simple_timer_start(&app_led_blinking_timer,
                                         APP_LED_BLINKING_TIMEOUT,
                                         app_led_blinking_timer_cb,
                                         NO_CALLBACK_DATA,
                                         true);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to start periodic timer\r\n",
                (int)sc);
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
  (void)address;
  (void)iv_index;
  sl_status_t sc = sl_simple_timer_stop(&app_led_blinking_timer);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to stop periodic timer\r\n",
                (int)sc);
  // Turn off LED
  init_done = true;
  sl_simple_led_turn_off(sl_led_led0.context);
#ifndef SINGLE_LED
  sl_simple_led_turn_off(sl_led_led1.context);
#endif // SINGLE_LED
  // Change LEDs to buttons in case of shared pin
  change_leds_to_buttons();
}
