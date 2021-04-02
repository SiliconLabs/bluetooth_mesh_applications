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
#include "sl_btmesh_button_press.h"

#include "sl_btmesh_data_logging_client.h"

/// Advertising Provisioning Bearer
#define PB_ADV                         0x1
/// GATT Provisioning Bearer
#define PB_GATT                        0x2

#define LOG_SAMPLE_RATE                1500
#define LOG_PERIOD                     5000

/// Buffer for the Log received
sl_data_log_data_t log_data_arr[SL_BTMESH_DATA_LOG_BUFF_SIZE_CFG_VAL];

sl_data_log_t log_data = {
    .index = 0,
    .data = log_data_arr
};

// Log received indication
bool log_received_flag = false;

const char name[] = "Log Client";

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
  sl_btmesh_button_press_enable();
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
                    "[E: 0x%04x] Failed to init node\n",
                    (int)sc);
      log_received_flag = false;
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
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_system_boot_id:
      sc = sl_bt_gatt_server_write_attribute_value(gattdb_device_name,
                                             0,
                                             strlen(name),
                                             (uint8_t *)name);
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to set device name\n",
                    (int)sc);
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
      sl_app_log("0x%X ", log_data.data[count]);
  }
  sl_app_log("]\r\n");
}
