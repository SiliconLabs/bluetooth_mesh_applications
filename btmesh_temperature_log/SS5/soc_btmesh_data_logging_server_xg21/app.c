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

#include "sl_btmesh_data_logging_server.h"

const char name[] = "Log Server";

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
      // Init Data logging server
      sc = sl_btmesh_data_log_server_init();
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to initialize data logging server\n",
                    (int)sc);

      if (!evt->data.evt_node_initialized.provisioned) {
        // The Node is now initialized,
        sl_app_log("Initialized\r\n");

        // start unprovisioned Beaconing using PB-ADV and PB-GATT Bearers
        sc = sl_btmesh_node_start_unprov_beaconing(0x3);
        sl_app_assert(sc == SL_STATUS_OK,
                      "[E: 0x%04x] Failed to start unprovisioned beaconing\n",
                      (int)sc);
      }
      break;
    case sl_btmesh_evt_vendor_model_receive_id:
      sc = sl_btmesh_data_log_on_server_receive_event(evt);
      sl_app_assert(sc == SL_STATUS_OK,
                      "[E: 0x%04x] Failed to process Log server event\n",
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
 * Button press Callbacks
 *******************************************************************************/
void sl_btmesh_button_press_cb(uint8_t button, uint8_t duration)
{
  sl_status_t sc;
  (void)duration;
  if (button == SL_BTMESH_BUTTON_PRESS_BUTTON_0) {
    sc = sl_btmesh_data_log_server_start();
    sl_app_assert(sc == SL_STATUS_OK,
                  "[E: 0x%04x] Failed to start Log\n",
                  (int)sc);
    sl_app_log("Log started\r\n");
  }
  else if(button == SL_BTMESH_BUTTON_PRESS_BUTTON_1){
      sl_btmesh_initiate_factory_reset();
  }
}

/***************************************************************************//**
 * Sample callback
 *******************************************************************************/
void sl_btmesh_data_log_on_sample_callback(void)
{
  static uint8_t counter = 0;

  // Fill simulated data to log
  sl_status_t sc = sl_btmesh_data_log_append((sl_data_log_data_t *)&counter);
  sl_app_assert(sc != SL_STATUS_FAIL,
                "[E: 0x%04x] Failed to append log\n",
                (int)sc);
  counter++;
}

/***************************************************************************//**
 * Log sending complete callback
 *******************************************************************************/
void sl_btmesh_data_log_complete_callback(void)
{
  sl_app_log("Log sent completely\r\n");
}

/***************************************************************************//**
 * Log full callback
 *******************************************************************************/
void sl_btmesh_data_log_full_callback(void)
{
  (void)sl_btmesh_data_log_reset();
}

/***************************************************************************//**
 * Log over flow callback
 *******************************************************************************/
void sl_btmesh_data_log_ovf_callback(void)
{
  (void)sl_btmesh_data_log_reset();
}
