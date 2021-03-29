/***************************************************************************//**
* @file sl_btmesh_data_logging_server.c
* @brief BT Mesh Data Logging Server Instances
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifdef __cplusplus
extern "C" {
#endif

#include "sl_sleeptimer.h"
#include "sl_app_log.h"
#include "sl_btmesh_data_logging_server.h"

/// Send data with length in the range of the BTMesh stack limitation
static sl_status_t sli_btmesh_data_log_send(sl_data_frame_t *frame,
                                     sl_data_log_length_t len);

/// Handle data transmission
static sl_status_t sli_btmesh_data_log_send_handler(void);

/// Timeout timer callback
static void sli_btmesh_data_log_timeout_callback(
            sl_sleeptimer_timer_handle_t *handle,
            void *data);

/// Sample timer callback
static void sli_btmesh_data_log_sample_callback(
            sl_sleeptimer_timer_handle_t *handle,
            void *data);

/// Periodic timer callback
static void sli_btmesh_data_log_periodic_callback(
            sl_sleeptimer_timer_handle_t *handle,
            void *data);

/// Update log period timing
static sl_status_t sli_btmesh_data_log_update_period(
                sl_btmesh_evt_vendor_model_receive_t *evt);

/// Update log sample rate timing
static sl_status_t sli_btmesh_data_log_update_sample_rate(
                sl_btmesh_evt_vendor_model_receive_t *evt);

/// Update log raw value
static sl_status_t sli_btmesh_data_log_update_raw(
                sl_btmesh_evt_vendor_model_receive_t *evt);

/// Data logging buffer
static sl_data_log_data_t sli_data_log_arr[SL_BTMESH_DATA_LOG_BUFF_SIZE_CFG_VAL];

/// Data logging structure
static sl_data_log_t sli_data_log_inst = {
  .index = 0,
  .data = sli_data_log_arr
};

/// Properties data structure
static sl_btmesh_data_log_properties_t sli_data_log_properties = {
    .sample_rate = SL_BTMESH_DATA_LOG_SAMPLE_RATE_MS_CFG_VAL,
    .period = SL_BTMESH_DATA_LOG_PERIOD_MS_CFG_VAL,
    .raw_data = SL_BTMESH_DATA_LOG_RAW_CFG_VAL
};

/// Counter for the log transmission
static uint16_t sli_send_count;
/// Position of sending data
sl_data_log_index_t sli_send_index;
/// Counter for the log transmission
static uint8_t sli_send_status;

/// Timer for sending timeout
static sl_sleeptimer_timer_handle_t sli_data_log_timeout_timer;
/// Timer for the log sample
static sl_sleeptimer_timer_handle_t sli_data_log_sample_timer;
/// Timer for the log report
static sl_sleeptimer_timer_handle_t sli_data_log_periodic_timer;

// Initialize model for element
sl_status_t sl_btmesh_data_log_server_init(void)
{
  sl_status_t st;
  st = sl_btmesh_vendor_model_init(SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL,
                                   SL_BTMESH_VENDOR_ID,
                                   SL_BTMESH_DATA_LOG_MODEL_SERVER_ID,
                                   SL_BTMESH_MODEL_PUBLISH,
                                   SL_BTMESH_DATA_LOG_OPCODE_LENGTH,
                                   sl_btmesh_data_log_opcodes);

  if(SL_STATUS_OK == st){
    // Clear log
    sli_data_log_inst.index = SL_BTMESH_DATA_LOG_RESET_VAL;
    memset(sli_data_log_inst.data,
           SL_BTMESH_DATA_LOG_CLEAR_VAL,
           SL_BTMESH_DATA_LOG_BUFF_SIZE_CFG_VAL*sizeof(sl_data_log_data_t));
  } else {
      return st;
  }

  // Reset transmission counter
  sli_send_count = SL_BTMESH_DATA_LOG_RESET_VAL;

  // Reset transmission status
  sli_send_status = SL_BTMESH_DATA_LOG_IDLE;

  return st;
}

sl_status_t sl_btmesh_data_log_server_deinit(void)
{
  sli_send_count = SL_BTMESH_DATA_LOG_RESET_VAL;
  // Stop timers
  sl_sleeptimer_stop_timer(&sli_data_log_timeout_timer);
  sl_sleeptimer_stop_timer(&sli_data_log_sample_timer);
  sl_sleeptimer_stop_timer(&sli_data_log_periodic_timer);

  // De-Init model
  return sl_btmesh_vendor_model_deinit(SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL,
                                     SL_BTMESH_VENDOR_ID,
                                     SL_BTMESH_DATA_LOG_MODEL_SERVER_ID);
}

static sl_status_t sli_btmesh_data_log_send(sl_data_frame_t *frame,
                                     sl_data_log_length_t len)
{
  sl_status_t st;

  if(len > SL_BTMESH_DATA_LOG_LENGTH_MAX){
      return SL_STATUS_INVALID_RANGE;
  }

  // Set the information log message
  st = sl_btmesh_vendor_model_set_publication(
                            SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL,
                            SL_BTMESH_VENDOR_ID,
                            SL_BTMESH_DATA_LOG_MODEL_SERVER_ID,
                            SL_BTMESH_DATA_LOG_MESSAGE_STATUS_ID,
                            SL_BTMESH_SEGMENT_CONTI,
                            SL_BTMESH_DATA_LOG_INFO_LENGTH,
                            (const uint8_t *)&(frame->last));
  if(SL_STATUS_OK == st){
    // Set the sending data log message
    st = sl_btmesh_vendor_model_set_publication(
                              SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL,
                              SL_BTMESH_VENDOR_ID,
                              SL_BTMESH_DATA_LOG_MODEL_SERVER_ID,
                              SL_BTMESH_DATA_LOG_MESSAGE_STATUS_ID,
                              SL_BTMESH_SEGMENT_FINAL,
                              len,
                              (const uint8_t *)frame->data);

    // Send the log
    if(SL_STATUS_OK == st){
      st = sl_btmesh_vendor_model_publish(SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL,
                                          SL_BTMESH_VENDOR_ID,
                                          SL_BTMESH_DATA_LOG_MODEL_SERVER_ID);
    }
  }

  return st;
}

sl_status_t sl_btmesh_data_log_server_start(void)
{
  sl_status_t st;

  // Start periodic timer
  st = sl_sleeptimer_start_periodic_timer_ms(
              &sli_data_log_periodic_timer,
              sli_data_log_properties.period,
              &sli_btmesh_data_log_periodic_callback,
              NO_CALLBACK_DATA,
              HIGH_PRIORITY,
              NO_FLAGS);

  if(SL_STATUS_OK != st){
      return st;
  }

  // Start sample timer
  st = sl_sleeptimer_start_periodic_timer_ms(
              &sli_data_log_sample_timer,
              sli_data_log_properties.sample_rate,
              &sli_btmesh_data_log_sample_callback,
              NO_CALLBACK_DATA,
              HIGH_PRIORITY,
              NO_FLAGS);

  return st;
}

sl_status_t sl_btmesh_data_log_server_send_status(void)
{
  sl_status_t st;
  sl_data_log_index_t idx = sli_data_log_inst.index;
  if(idx > 0){
      if (SL_BTMESH_DATA_LOG_BUSY == sli_send_status){
          st = SL_STATUS_BUSY;
      } else {
        // Calculate number of segment need to be sent
        sli_send_count = SL_BTMESH_DATA_LOG_SEG_NUM(idx);

        // Start sending
        st = sli_btmesh_data_log_send_handler();
      }
  } else {
      st = SL_STATUS_EMPTY;
      sl_app_log("The Log is empty\r\n");
  } // The log is empty

  return st;
}

sl_status_t sli_btmesh_data_log_send_handler(void)
{
  sl_status_t st;
  sl_data_log_length_t len;
  sl_data_frame_t data_frame;

  if(sli_send_count > 0){
      if(SL_BTMESH_DATA_LOG_IDLE == sli_send_status){
          sli_send_status = SL_BTMESH_DATA_LOG_BUSY;
          // Start sending from the beginning of the log
          sli_send_index = SL_BTMESH_DATA_LOG_RESET_VAL;

          // Start timer
          st = sl_sleeptimer_start_timer_ms(&sli_data_log_timeout_timer,
                                      SL_BTMESH_DATA_LOG_TIMEOUT_MS_CFG_VAL,
                                      &sli_btmesh_data_log_timeout_callback,
                                      NO_CALLBACK_DATA,
                                      HIGH_PRIORITY,
                                      NO_FLAGS);
          if(SL_STATUS_OK != st){
              return st;
          }
      }
      if(sli_send_count > 1){
          len = SL_BTMESH_DATA_LOG_LENGTH_MAX
                * sizeof(sl_data_log_data_t);

          // Send segment
          data_frame.last = SL_BTMESH_DATA_LOG_NOT_LAST;
          data_frame.data = &(sli_data_log_inst.data[sli_send_index]);
          st = sli_btmesh_data_log_send(&data_frame, len);

          if(SL_STATUS_OK == st){
              // Point to the next segment
              sli_send_index += SL_BTMESH_DATA_LOG_LENGTH_MAX;
              sli_send_count--;
          } else { // Send failed
              // Reset transmission
              sli_send_count = SL_BTMESH_DATA_LOG_RESET_VAL;
              sli_send_status = SL_BTMESH_DATA_LOG_IDLE;
              sl_app_log("Failed to send Log\r\n");
              return st;
          }
      } else { // (sli_send_count == 1) Last segment
          len = (sli_data_log_inst.index - sli_send_index)
                * sizeof(sl_data_log_data_t);
          // Send the last segment
          data_frame.last = SL_BTMESH_DATA_LOG_LAST;
          data_frame.data = &(sli_data_log_inst.data[sli_send_index]);
          st = sli_btmesh_data_log_send(&data_frame, len);
          if(SL_STATUS_OK == st){
              sli_send_count--;
          } else { // Send fail
              // Reset transmission
              sli_send_count = SL_BTMESH_DATA_LOG_RESET_VAL;
              sli_send_status = SL_BTMESH_DATA_LOG_IDLE;
              sl_app_log("Failed to send Log\r\n");
              return st;
          }
      }
  } else if(SL_BTMESH_DATA_LOG_BUSY == sli_send_status){
      // Reset log
      sli_data_log_inst.index = SL_BTMESH_DATA_LOG_RESET_VAL;
      sli_send_status = SL_BTMESH_DATA_LOG_IDLE;
      sl_sleeptimer_stop_timer(&sli_data_log_timeout_timer);
      sl_btmesh_data_log_complete_callback();
      st = SL_STATUS_OK;
  } else { st = SL_STATUS_OK; }

  return st;
}

/* Append new data to log */
sl_status_t sl_btmesh_data_log_append(sl_data_log_data_t *data)
{
  sl_data_log_data_t *ret_ptr;
  sl_data_log_index_t idx;

  idx = sli_data_log_inst.index;
  if(idx < SL_BTMESH_DATA_LOG_BUFF_SIZE_CFG_VAL) {
    ret_ptr = (sl_data_log_data_t *)memcpy((sl_data_log_data_t *)&(sli_data_log_inst.data[idx]),
                                           (sl_data_log_data_t *)data,
                                           sizeof(sl_data_log_data_t));
    if(NULL == ret_ptr) {
      return SL_STATUS_FAIL;
    }
  }

  if(idx < (SL_BTMESH_DATA_LOG_BUFF_SIZE_CFG_VAL - 1)) {
      sli_data_log_inst.index++;
      return SL_STATUS_OK;
  } else if(idx == (SL_BTMESH_DATA_LOG_BUFF_SIZE_CFG_VAL - 1)) {
      // Execute full callback function
      sl_btmesh_data_log_full_callback();
      sli_data_log_inst.index++;
      return SL_STATUS_FULL;
  } else {
      // Execute overflow callback function
      sl_app_log("The log exceeds limit!");
      sl_btmesh_data_log_ovf_callback();
  }

  return SL_STATUS_FAIL;
}

sl_status_t sl_btmesh_data_log_reset(void)
{
  if(SL_BTMESH_DATA_LOG_IDLE == sli_send_status){
      sli_data_log_inst.index = SL_BTMESH_DATA_LOG_RESET_VAL;
  } else {
      return SL_STATUS_BUSY;
  }

  return SL_STATUS_OK;
}

uint8_t sl_btmesh_data_log_get_server_state(void)
{
  return sli_send_status;
}

sl_status_t sl_btmesh_data_log_on_server_receive_event(sl_btmesh_msg_t *evt)
{
  sl_status_t st;
  sl_btmesh_evt_vendor_model_receive_t *log_evt =
          &(evt->data.evt_vendor_model_receive);
  switch(log_evt->opcode)
  {
    case SL_BTMESH_DATA_LOG_MESSAGE_STATUS_RSP_ID:
      st = sli_btmesh_data_log_send_handler();
      break;
    case SL_BTMESH_DATA_LOG_MESSAGE_PERIOD_ID:
      st = sli_btmesh_data_log_update_period(log_evt);
      break;
    case SL_BTMESH_DATA_LOG_MESSAGE_SAMPLE_RATE_ID:
      st = sli_btmesh_data_log_update_sample_rate(log_evt);
      break;
    case SL_BTMESH_DATA_LOG_MESSAGE_RAW_ID:
      st = sli_btmesh_data_log_update_raw(log_evt);
      break;
    default: st = SL_STATUS_FAIL;
  }

  return st;
}

static void sli_btmesh_data_log_timeout_callback(
            sl_sleeptimer_timer_handle_t *handle,
            void *data)
{
  (void)handle;
  (void)data;

  // Transmission timeout
  if(SL_BTMESH_DATA_LOG_BUSY == sli_send_status){
      sli_send_count = SL_BTMESH_DATA_LOG_RESET_VAL;
      sli_send_status = SL_BTMESH_DATA_LOG_IDLE;
      sl_app_log("Log sending timeout!\r\n");
  }
}

static void sli_btmesh_data_log_sample_callback(
            sl_sleeptimer_timer_handle_t *handle,
            void *data)
{
  (void)handle;
  (void)data;

  sl_btmesh_data_log_on_sample_callback();
}

static void sli_btmesh_data_log_periodic_callback(
            sl_sleeptimer_timer_handle_t *handle,
            void *data)
{
  (void)handle;
  (void)data;

  sl_app_log("Periodic send Log status\n");
  (void)sl_btmesh_data_log_server_send_status();
}

static sl_status_t sli_btmesh_data_log_update_period(
                sl_btmesh_evt_vendor_model_receive_t *evt)
{
  sl_status_t st;
  memcpy((uint8_t *)&sli_data_log_properties.period,
         (uint8_t *)evt->payload.data,
         sizeof(sl_btmesh_data_log_period_t));

  // Re-Start periodic timer
  st = sl_sleeptimer_restart_periodic_timer_ms(
              &sli_data_log_periodic_timer,
              sli_data_log_properties.period,
              &sli_btmesh_data_log_periodic_callback,
              NO_CALLBACK_DATA,
              HIGH_PRIORITY,
              NO_FLAGS);

  sl_btmesh_data_log_on_properties_callback();

  return st;
}

static sl_status_t sli_btmesh_data_log_update_sample_rate(
                sl_btmesh_evt_vendor_model_receive_t *evt)
{
  sl_status_t st;
  memcpy((uint8_t *)&sli_data_log_properties.sample_rate,
         (uint8_t *)evt->payload.data,
         sizeof(sl_btmesh_data_log_sample_rate_t));

  // Re-Start sample timer
  st = sl_sleeptimer_restart_periodic_timer_ms(
              &sli_data_log_sample_timer,
              sli_data_log_properties.sample_rate,
              &sli_btmesh_data_log_sample_callback,
              NO_CALLBACK_DATA,
              HIGH_PRIORITY,
              NO_FLAGS);

  sl_btmesh_data_log_on_properties_callback();

  return st;
}

static sl_status_t sli_btmesh_data_log_update_raw(
                sl_btmesh_evt_vendor_model_receive_t *evt)
{
  if(NULL != memcpy((uint8_t *)&sli_data_log_properties.raw_data,
                    (uint8_t *)evt->payload.data,
                    sizeof(sl_btmesh_data_log_raw_t)))
  {
    sl_btmesh_data_log_on_properties_callback();
    return SL_STATUS_OK;
  } else { return SL_STATUS_FAIL; }
}

sl_btmesh_data_log_raw_t sl_btmesh_data_log_get_raw(void)
{
  return sli_data_log_properties.raw_data;
}

////////////////////////////////////////////////////////////////////////////////
// Weak implementation of Callbacks                                           //
////////////////////////////////////////////////////////////////////////////////
SL_WEAK void sl_btmesh_data_log_full_callback(void)
{

}

SL_WEAK void sl_btmesh_data_log_ovf_callback(void)
{

}

SL_WEAK void sl_btmesh_data_log_complete_callback(void)
{

}

SL_WEAK void sl_btmesh_data_log_on_sample_callback(void)
{

}

SL_WEAK void sl_btmesh_data_log_on_properties_callback(void)
{

}

#ifdef __cplusplus
}
#endif
