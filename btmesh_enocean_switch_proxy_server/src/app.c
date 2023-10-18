/*****************************************************************************
 * @file
 * @brief Top level application functions
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
#include "enocean_proxy_api.h"

#include "app_button_press.h"
#include "app_log.h"
#include "sl_bt_api.h"
#include "sl_btmesh_api.h"
#include "sl_btmesh_factory_reset.h"
#include "sl_simple_button_instances.h"
#include <stdio.h>

#if defined SL_CATALOG_BTMESH_WSTK_LCD_PRESENT
#include "sl_btmesh_wstk_lcd.h"
#define lcd_print(...) sl_btmesh_LCD_write(__VA_ARGS__)
#else
#define lcd_print(...)
#endif // SL_CATALOG_BTMESH_WSTK_LCD_PRESENT

static bool handle_reset_conditions(void)
{
#ifdef SL_CATALOG_BTMESH_FACTORY_RESET_PRESENT
  // If PB0 is held down then do full factory reset
  if (sl_simple_button_get_state(&sl_button_btn0)
      == SL_SIMPLE_BUTTON_PRESSED) {
    // Full factory reset
    sl_btmesh_initiate_full_reset();
    return false;
  }

#if SL_SIMPLE_BUTTON_COUNT >= 2
  // If PB1 is held down then do node factory reset
  if (sl_simple_button_get_state(&sl_button_btn1)
      == SL_SIMPLE_BUTTON_PRESSED) {
    // Node factory reset
    sl_btmesh_initiate_node_reset();
    return false;
  }
#endif // SL_CATALOG_BTN1_PRESENT
#endif // SL_CATALOG_BTMESH_FACTORY_RESET_PRESENT
  return true;
}

void app_init(void)
{
  app_button_press_init();
  app_button_press_enable();
  handle_reset_conditions();
}

void app_process_action(void)
{
}

void sl_btmesh_on_event(sl_btmesh_msg_t *evt)
{
  // sl_status_t status;
  switch (SL_BGAPI_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_initialized_id:
      if (evt->data.evt_node_initialized.provisioned) {
        lcd_print("provisioned", 2);
      } else {
        lcd_print("unprovisioned", 2);
      }
      break;
    case sl_btmesh_evt_node_provisioned_id:
      lcd_print("provisioned", 2);
      break;
    default:
      break;
  }
}

void app_button_press_cb(uint8_t button, uint8_t duration)
{
  (void)button;
  (void)duration;
  uint16_t element_index = enocean_proxy_get_lowest_unused_element_index();
  if (element_index > 0xff) {
    app_log("Can't enter commissioning mode: no free elements\n");
  }
  enocean_proxy_enter_commissioning_mode((uint8_t)element_index);
}

void enocean_proxy_num_switches_changed(uint8_t new_switch_count)
{
  if (new_switch_count > 0) {
    char switch_count_string[25];
    snprintf(switch_count_string, 25, "num switches: %u", new_switch_count);
    lcd_print(switch_count_string, 3);
  } else {
    lcd_print("no switches", 3);
  }
}

void enocean_proxy_commissioning_mode_entered(void)
{
  lcd_print("Commissioning mode", 4);
}

void enocean_proxy_commissioning_mode_exited(void)
{
  lcd_print("", 4);
}
