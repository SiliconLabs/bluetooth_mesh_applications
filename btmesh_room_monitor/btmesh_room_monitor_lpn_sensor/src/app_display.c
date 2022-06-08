/***************************************************************************//**
 * @file app_display.c
 * @brief Application display module
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
#include "stdio.h"
#include "string.h"
#include "sl_status.h"

#include "app_log.h"
#include "app_assert.h"

#include "glib.h"
#include "app_display.h"

#define LINE_MAX_CHAR (8)

enum DISPLAY_PAGE {
  DISPLAY_PAGE_NONE,
  DISPLAY_PAGE_UNPROVISIONED,
  DISPLAY_PAGE_PEOPLE_COUNT
};

static enum DISPLAY_PAGE current_display_page = DISPLAY_PAGE_NONE;

static glib_context_t glib_context;

/// Bluetooth address
static bd_addr ble_addr;

// Display runtime data
unprovisioned_display_data_t display;

/// periodic timer callback
static void app_simple_timer_display_cb(sl_simple_timer_t *handle, void *data);

static void init_display_page(enum DISPLAY_PAGE display_page);

/* This array is generated by LCDAssistant Tool. A image with bitmap format can
 * use this tool to convert to code.
 * To draw an image or custom bitmaps on the oled, the glib_draw_bmp()
 * function can be used.
 * */
const uint8_t silicon_labs_logo[] = {
  0xFF, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x81,
  0x81, 0x81, 0x81, 0x81, 0x01, 0x41, 0x01, 0x01, 0x01, 0x01, 0x01, 0x81, 0x81,
  0x81, 0x81, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x81, 0x81, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xE0, 0xF0, 0xF0, 0xF8,
  0xF8, 0x3C, 0x1C, 0x0C, 0x06, 0x06, 0x02, 0x02, 0x03, 0x01, 0x01, 0x61, 0xF0,
  0xF8, 0xFC, 0xFC, 0x7E, 0x7E, 0x7F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
  0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x7E, 0x76, 0x60, 0xE0, 0xE0, 0xE0, 0xF0,
  0xF0, 0xF8, 0xFD, 0xFF, 0xFF, 0xFE, 0xFC, 0x78, 0x00, 0x00, 0xFF, 0xFF, 0x00,
  0x00, 0x00, 0x3C, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x1F,
  0x0E, 0x0C, 0x1C, 0x1C, 0xFC, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8,
  0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xFD, 0xFF, 0x7F, 0x7E, 0x3E, 0x1C,
  0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x40, 0x40, 0x60, 0x30, 0x39, 0x1F, 0x1F,
  0x0F, 0x0F, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x06, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xCC, 0x12,
  0x12, 0x12, 0xE4, 0x00, 0xFE, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x00,
  0x78, 0x84, 0x02, 0x02, 0x02, 0xCC, 0x00, 0x78, 0x84, 0x02, 0x02, 0x84, 0x78,
  0x00, 0xFE, 0x04, 0x18, 0x60, 0x80, 0xFE, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00,
  0x00, 0x00, 0x80, 0x7C, 0x42, 0x7C, 0x80, 0x00, 0xFE, 0x12, 0x12, 0x12, 0xEC,
  0x00, 0xCC, 0x12, 0x12, 0x12, 0xE4, 0x00, 0xFF, 0xFF, 0x80, 0x80, 0x81, 0x81,
  0x81, 0x80, 0x80, 0x81, 0x80, 0x81, 0x81, 0x81, 0x81, 0x80, 0x81, 0x80, 0x80,
  0x80, 0x81, 0x81, 0x81, 0x80, 0x80, 0x80, 0x80, 0x81, 0x81, 0x80, 0x80, 0x80,
  0x81, 0x80, 0x80, 0x80, 0x80, 0x81, 0x80, 0x80, 0x80, 0x81, 0x81, 0x81, 0x81,
  0x80, 0x81, 0x80, 0x80, 0x80, 0x81, 0x80, 0x81, 0x81, 0x81, 0x81, 0x80, 0x80,
  0x80, 0x81, 0x81, 0x81, 0x80, 0x80, 0xFF
  };

/***************************************************************************//**
 * Initialize the AIR QUALITY application.
 ******************************************************************************/
void app_display_init(void)
{
  /* Initialize the OLED display */
  glib_init();

  glib_context.backgroundColor = Black;
  glib_context.foregroundColor = White;
  display.max_char_per_line = (uint8_t) (64
          / (glib_font_7x10.width + glib_font_7x10.spacing));

  /* Fill lcd with background color */
  glib_clear(&glib_context);

  glib_draw_bmp(&glib_context, silicon_labs_logo);
  sl_sleeptimer_delay_millisecond(1000);
}

/***************************************************************************//**
 * Get the Bluetooth address
 ******************************************************************************/
void app_display_set_name(bd_addr addr)
{
  ble_addr = addr;
}

/***************************************************************************//**
 * Draw screen when device is unprovisioned.
 ******************************************************************************/
void app_display_show_unprovisioned(void)
{
  char name[6];

  // Init display page
  init_display_page(DISPLAY_PAGE_UNPROVISIONED);

  sprintf(name, "%02x:%02x", ble_addr.addr[1], ble_addr.addr[0]);

  glib_set_font(&glib_context, (glib_font_t *) &glib_font_7x10);
  /* Fill lcd with background color */
  glib_clear(&glib_context);
  glib_draw_line(&glib_context, 0, 15, 63, 15);
  glib_draw_string(&glib_context, "ID ADDR:", 0, 23);
  glib_draw_string(&glib_context, name, 12, 35);

  strcpy((char*) display.param_text, "UNPROVISIONED");
  // Set animation parameters
  display.first_letter = display.param_text;
  display.text_length = strlen((char*) display.param_text);

  glib_update_display();

  // Setup a timer for display animations
  sl_status_t sc = sl_simple_timer_start(&display.timer_handle,
                                         300,
                                         app_simple_timer_display_cb,
                                         (void *)NULL,
                                         true);
  app_assert_status_f(sc, "Failed to start timer\r\n");
}

/***************************************************************************//**
 * Draw screen when device is factory reset.
 ******************************************************************************/
void app_display_show_factory_reset(void)
{
  glib_set_font(&glib_context, (glib_font_t *) &glib_font_7x10);

  // Fill lcd with background color
  glib_clear(&glib_context);
  glib_draw_string(&glib_context, "FACTORY", 4, 15);
  glib_draw_string(&glib_context, "RESET", 12, 30);
  glib_update_display();
}

/***************************************************************************//**
 * Draw screen when device is provisioned.
 ******************************************************************************/
void app_display_show_people_count(uint32_t people_count)
{
  char number_str[9];
  char tmp[11];
  int n;

  // Init display page
  init_display_page(DISPLAY_PAGE_PEOPLE_COUNT);

  // To keep the people count number in the middle of the screen
  n = snprintf(number_str, sizeof(number_str), "%lu", people_count);
  
  if (n < LINE_MAX_CHAR) {
    const char blank[] = "         ";
    int n_blank = (LINE_MAX_CHAR - n) / 2;
    snprintf(tmp, sizeof(tmp), "%.*s%s%.*s", n_blank, blank,
                                             number_str,
                                             LINE_MAX_CHAR - n - n_blank,
                                             blank);
    glib_set_font(&glib_context, (glib_font_t *) &glib_font_7x10);
    glib_draw_string(&glib_context, tmp, 0, 19);
  }
  else {
    glib_set_font(&glib_context, (glib_font_t *) &glib_font_7x10);
    glib_draw_string(&glib_context, number_str, 0, 19);
  }

  glib_update_display();
}

/***************************************************************************//**
 * Draw screen when device is provisioned.
 ******************************************************************************/
void app_display_show_friend_status(bool status) {
  static bool friend_status = false;
  char tmp[11];

  if(friend_status != status) {
    friend_status = status;

    // Init display page
    init_display_page(DISPLAY_PAGE_PEOPLE_COUNT);

    snprintf(tmp, sizeof(tmp), "FRIEND:%s", status ? "YES":"NO ");
    glib_set_font(&glib_context, (glib_font_t *) &glib_font_6x8);
    glib_draw_string(&glib_context, tmp, 0, 37);

    glib_update_display();
  }
}

static void init_display_page(enum DISPLAY_PAGE display_page)
{
  if(current_display_page != display_page) {
    /* Fill lcd with background color */
    glib_clear(&glib_context);
    sl_simple_timer_stop(&display.timer_handle);

    current_display_page = display_page;

    switch(display_page) {

      case DISPLAY_PAGE_PEOPLE_COUNT:
        glib_set_font(&glib_context, (glib_font_t *) &glib_font_6x8);
        glib_draw_string(&glib_context, "PEOPLE", 15, 0);

        glib_draw_line(&glib_context, 0, 11, 63, 11);
        glib_draw_line(&glib_context, 0, 33, 63, 33);
        break;

      default:
        break;
    }
  }
}

static void display_set_paramline_next_text(uint8_t *text_buffer)
{
  uint8_t txt_length = strlen((char*) display.first_letter);
  uint8_t txt_remainder_length = 0;

  if (txt_length > display.max_char_per_line) {
    txt_length = display.max_char_per_line;
  } else {
    txt_remainder_length = display.max_char_per_line
        - txt_length;
  }

  // Copy letters starting from first_letter pointer
  memcpy(text_buffer, display.first_letter, txt_length);

  // Copy remaining letters if any
  if (0 != txt_remainder_length) {
    // Append a whitespace between the last and first characters
    text_buffer[txt_length] = ' ';
    memcpy(&text_buffer[txt_length + 1], display.param_text,
        txt_remainder_length);
  }

  // Advance first letter pointer
  display.first_letter++;

  // If it points to the tailing char then
  if (*display.first_letter == '\0') {
    display.first_letter = display.param_text;
  }
  text_buffer[display.max_char_per_line] = '\0';
}

/***************************************************************************//**
 * Timer Callbacks
 ******************************************************************************/
static void app_simple_timer_display_cb(sl_simple_timer_t *handle,
                                        void *data)
{
  (void)data;
  (void)handle;
  uint8_t param_text_buffer[display.max_char_per_line + 1];

  display_set_paramline_next_text(param_text_buffer);
  glib_draw_string(&glib_context, (char*) param_text_buffer, 1, 4);
  // Update display
  glib_update_display();
}
