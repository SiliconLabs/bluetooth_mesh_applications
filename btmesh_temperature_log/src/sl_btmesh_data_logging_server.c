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
*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "sl_app_assert.h"
#include "sl_sleeptimer.h"
#include "sl_app_log.h"
#include "sl_btmesh_data_logging_server.h"

#include "nvm3.h"
#include "nvm3_hal_flash.h"

/***************************************************************************//**
 *
 * Macros
 *
 ******************************************************************************/
#define SLI_BTMESH_NVM3_DATA_LOG_PROP_KEY   0

/***************************************************************************//**
 *
 * Internal functions prototype
 *
 ******************************************************************************/
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
static sl_status_t sli_btmesh_data_log_update_threshold(
                sl_btmesh_evt_vendor_model_receive_t *evt);

/// Save period to NVM
static sl_status_t data_log_save_period(sl_btmesh_data_log_period_t period);

/// Save sample rate to NVM
static sl_status_t data_log_save_sample_rate(
                      sl_btmesh_data_log_sample_rate_t rate);

/// Save threshold to NVM
static sl_status_t data_log_save_threshold(
                      sl_btmesh_data_log_threshold_t threshold);

/// Read period stored in NVM
static sl_status_t data_log_read_period(sl_btmesh_data_log_period_t *period);

/// Read sample rate stored in NVM
static sl_status_t data_log_read_sample_rate(
                      sl_btmesh_data_log_sample_rate_t *rate);

/// Read threshold stored in NVM
static sl_status_t data_log_read_threshold(
                      sl_btmesh_data_log_threshold_t *threshold);

/// Check for the valid storage object
static bool is_storage_valid(void);

/// Read the properties structure from the NVM.
__STATIC_INLINE Ecode_t data_log_read_properties(
                      sl_btmesh_data_log_properties_t *properties);

/// Write the properties structure from the NVM.
__STATIC_INLINE Ecode_t data_log_write_properties(
                      sl_btmesh_data_log_properties_t *properties);

/***************************************************************************//**
 *
 * Global variables
 *
 ******************************************************************************/
/// Data logging buffer
static sl_data_log_data_t sli_data_log_arr[SL_BTMESH_DATA_LOG_BUFF_SIZE_CFG_VAL];

/// Data logging instance
static sl_data_log_t sli_data_log_inst = {
  .index = 0,
  .data = sli_data_log_arr
};

/// Counter for the log transmission
static uint16_t sli_send_count;
/// Position of sending data
static sl_data_log_index_t sli_send_index;
/// Counter for the log transmission
static uint8_t sli_send_status;
/// The Log status
static bool sli_log_started;
/// Start to send log
static bool is_sending_started;
/// Transmit counter
static uint8_t trans_count;

/// Timer for sending timeout
static sl_sleeptimer_timer_handle_t sli_data_log_timeout_timer;
/// Timer for the log sample
static sl_sleeptimer_timer_handle_t sli_data_log_sample_timer;
/// Timer for the log report
static sl_sleeptimer_timer_handle_t sli_data_log_periodic_timer;

/***************************************************************************//**
 *
 * Functions implementation
 *
 ******************************************************************************/
/***************************************************************************//**
 * Initialize the data log server.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_server_init(void)
{
  sl_status_t st;
  Ecode_t ec;
  sl_btmesh_data_log_properties_t properties;

  sli_log_started = false;
  is_sending_started = false;
  //Init vendor model
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

  // Init NVM too store configuration data
  if(!is_storage_valid()){
      // Init with default values
      properties.period = SL_BTMESH_DATA_LOG_PERIOD_MS_CFG_VAL;
      properties.sample_rate = SL_BTMESH_DATA_LOG_SAMPLE_RATE_MS_CFG_VAL;
      properties.threshold = SL_BTMESH_DATA_LOG_THESHOLD_CFG_VAL;
      ec = data_log_write_properties(&properties);
      sl_app_assert(ec == ECODE_NVM3_OK,
                "[E: 0x%08x] Failed to create NVM storage\n",
                (int)ec);
  }

  // Reset transmission counter
  sli_send_count = SL_BTMESH_DATA_LOG_RESET_VAL;

  // Reset transmission status
  sli_send_status = SL_BTMESH_DATA_LOG_IDLE;
  trans_count = 0;

  return st;
}

/***************************************************************************//**
 * De-Initialize the data log client.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_server_deinit(void)
{
  sli_send_count = SL_BTMESH_DATA_LOG_RESET_VAL;
  // Stop timers
  sl_sleeptimer_stop_timer(&sli_data_log_timeout_timer);
  sl_sleeptimer_stop_timer(&sli_data_log_sample_timer);
  sl_sleeptimer_stop_timer(&sli_data_log_periodic_timer);

  sli_log_started = false;
  is_sending_started = false;
  // De-Init vendor model
  return sl_btmesh_vendor_model_deinit(SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL,
                                     SL_BTMESH_VENDOR_ID,
                                     SL_BTMESH_DATA_LOG_MODEL_SERVER_ID);
}

/***************************************************************************//**
 * Initialize the data log client.
 *
 * @param[in] frame Pointer to send package instance.
 * @param[in] len Length of the data to be sent.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
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
                            (const uint8_t *)&(frame->header.last));
  sl_app_assert(st == SL_STATUS_OK,
                "[E: 0x%04x] Failed to set Log info publication\n",
                (int)st);

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
    sl_app_assert(st == SL_STATUS_OK,
                  "[E: 0x%04x] Failed to set Log publication\n",
                  (int)st);

    // Send the log
    if(SL_STATUS_OK == st){
      st = sl_btmesh_vendor_model_publish(SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL,
                                          SL_BTMESH_VENDOR_ID,
                                          SL_BTMESH_DATA_LOG_MODEL_SERVER_ID);
      sl_app_assert(st == SL_STATUS_OK,
                  "[E: 0x%04x] Failed to send Log publication\n",
                  (int)st);
    }
  }

  return st;
}

/***************************************************************************//**
 * Start the sample rate timer and Log period timer.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_server_start(void)
{
  sl_status_t st;
  sl_btmesh_data_log_period_t period = 0;
  sl_btmesh_data_log_sample_rate_t rate = 0;

  if(!sli_log_started){
    if(is_storage_valid()){
        // Get the saved period value
        st = data_log_read_period(&period);
        if(SL_STATUS_OK != st){
            return st;
        }
        st = data_log_read_sample_rate(&rate);
        if(SL_STATUS_OK != st){
            return st;
        }
    }
    // Storage is not valid. Use the default value.
    if(0 == period){
      period = SL_BTMESH_DATA_LOG_PERIOD_MS_CFG_VAL;
    }
    if(0 == rate){
      rate = SL_BTMESH_DATA_LOG_SAMPLE_RATE_MS_CFG_VAL;
    }

    // Start periodic timer
    st = sl_sleeptimer_start_periodic_timer_ms(
                &sli_data_log_periodic_timer,
                (uint32_t)period,
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
                (uint32_t)rate,
                &sli_btmesh_data_log_sample_callback,
                NO_CALLBACK_DATA,
                HIGH_PRIORITY,
                NO_FLAGS);

    if(SL_STATUS_OK != st){
        sli_log_started = true;
    }
  } else { return SL_STATUS_OK; }

  return st;
}

/***************************************************************************//**
 * Publish the current data.
 *
 * @param[in] data pointer to data to be sent.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_server_send_data(sl_data_log_data_t *data)
{
  sl_status_t st;
  // Set the sending data message
  st = sl_btmesh_vendor_model_set_publication(
                            SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL,
                            SL_BTMESH_VENDOR_ID,
                            SL_BTMESH_DATA_LOG_MODEL_SERVER_ID,
                            SL_BTMESH_DATA_LOG_MESSAGE_TEMP_ID,
                            SL_BTMESH_SEGMENT_FINAL,
                            sizeof(sl_data_log_data_t),
                            (const uint8_t *)data);
  sl_app_assert(st == SL_STATUS_OK,
                "[E: 0x%04x] Failed to set data publication\n",
                (int)st);

  // Send the log
  if(SL_STATUS_OK == st){
    st = sl_btmesh_vendor_model_publish(SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL,
                                        SL_BTMESH_VENDOR_ID,
                                        SL_BTMESH_DATA_LOG_MODEL_SERVER_ID);
    sl_app_assert(st == SL_STATUS_OK,
                "[E: 0x%04x] Failed to publish data\n",
                (int)st);
  }

  return st;
}

/***************************************************************************//**
 * Start to publish the Log.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
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
        if(!is_sending_started){
            is_sending_started = true;
        }
      }
  } else {
      st = SL_STATUS_EMPTY;
      sl_app_log("The Log is empty\r\n");
  } // The log is empty

  return st;
}

/***************************************************************************//**
 * Check the log sending is started.
 *
 * @return returns the log sending status.
 *         - true if log is requested to send
 *         - false if log is idle
 *
 ******************************************************************************/
bool sl_btmesh_data_log_is_started_sending(void)
{
  return is_sending_started;
}

/***************************************************************************//**
 * Process log sending.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_step(void)
{
  if(is_sending_started){
      return sli_btmesh_data_log_send_handler();
  }
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Handle the packages sending.
 *
 * Start of sending, handler will trigger a timeout timer. Within the timeout,
 * if packages send completely, a callback function will be call. Otherwise,
 * a timeout occurs and the sending state will be reset.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
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
          data_frame.header.last = SL_BTMESH_DATA_LOG_NOT_LAST;
          data_frame.header.count = trans_count;
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
              is_sending_started = false;
              sl_app_log("Failed to send Log\r\n");
              return st;
          }
      } else { // (sli_send_count == 1) Last segment
          len = (sli_data_log_inst.index - sli_send_index)
                * sizeof(sl_data_log_data_t);
          // Send the last segment
          data_frame.header.last = SL_BTMESH_DATA_LOG_LAST;
          data_frame.header.count = trans_count;
          data_frame.data = &(sli_data_log_inst.data[sli_send_index]);
          st = sli_btmesh_data_log_send(&data_frame, len);
          if(SL_STATUS_OK == st){
              sli_send_count--;
          } else { // Send fail
              // Reset transmission
              sli_send_count = SL_BTMESH_DATA_LOG_RESET_VAL;
              sli_send_status = SL_BTMESH_DATA_LOG_IDLE;
              is_sending_started = false;
              sl_app_log("Failed to send last Log\r\n");
              return st;
          }
      }
  } else if(SL_BTMESH_DATA_LOG_BUSY == sli_send_status){
      // Reset log
      sli_data_log_inst.index = SL_BTMESH_DATA_LOG_RESET_VAL;
      sli_send_status = SL_BTMESH_DATA_LOG_IDLE;
      is_sending_started = false;
      sl_sleeptimer_stop_timer(&sli_data_log_timeout_timer);
      // Execute complete callback
      sl_btmesh_data_log_complete_callback();
      // Next sending ID
      trans_count++;
      st = SL_STATUS_OK;
  } else { st = SL_STATUS_OK; }

  return st;
}

/***************************************************************************//**
 * Append new data to the Log.
 *
 * @param[in] data Data value to be appended to the Log.
 *
 * If the Log is full then a full callback is executed.
 * If the Log is overflow then a overflow callback is executed.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_append(sl_data_log_data_t *data)
{
  sl_data_log_data_t *ret_ptr;
  sl_data_log_index_t idx;

  // Check if the log is sanding
  if(SL_BTMESH_DATA_LOG_BUSY == sli_send_status){
      return SL_STATUS_BUSY;
  }
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

/***************************************************************************//**
 * Reset the Log.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_reset(void)
{
  if(SL_BTMESH_DATA_LOG_IDLE == sli_send_status){
      sli_data_log_inst.index = SL_BTMESH_DATA_LOG_RESET_VAL;
  } else {
      return SL_STATUS_BUSY;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Get the current status of the Log.
 *
 * @return Return the current status of the Log.
 *  - SL_BTMESH_DATA_LOG_IDLE if Log is idle.
 *  - SL_BTMESH_DATA_LOG_BUSY if Log is receiving.
 *
 ******************************************************************************/
uint8_t sl_btmesh_data_log_get_server_state(void)
{
  return sli_send_status;
}

/***************************************************************************//**
 * Handle event of the Log server.
 *
 * @param[in] evt Pointer to btmesh message.
 *
 * Handle event of the Vendor model. If there's valid opcode received then
 * receiving handler will be called for:
 * - Log received response.
 * - Set period request.
 * - Set sample rate request.
 * - Set threshold request.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 *  @note This function must be executed in sl_btmesh_on_event for the
 *  sl_btmesh_evt_vendor_model_receive_id event.
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_on_server_receive_event(sl_btmesh_msg_t *evt)
{
  sl_status_t st;
  sl_btmesh_evt_vendor_model_receive_t *log_evt =
          &(evt->data.evt_vendor_model_receive);
  switch(log_evt->opcode)
  {
#if defined(SL_BTMESH_DATA_LOG_RSP_ENABLE)
    case SL_BTMESH_DATA_LOG_MESSAGE_STATUS_RSP_ID:
      st = sli_btmesh_data_log_send_handler();
      break;
#endif
    case SL_BTMESH_DATA_LOG_MESSAGE_PERIOD_ID:
      st = sli_btmesh_data_log_update_period(log_evt);
      break;
    case SL_BTMESH_DATA_LOG_MESSAGE_SAMPLE_RATE_ID:
      st = sli_btmesh_data_log_update_sample_rate(log_evt);
      break;
    case SL_BTMESH_DATA_LOG_MESSAGE_THRESHOLD_ID:
      st = sli_btmesh_data_log_update_threshold(log_evt);
      break;
    default: st = SL_STATUS_FAIL;
  }

  return st;
}

/***************************************************************************//**
 * Timeout timer callback function.
 *
 ******************************************************************************/
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
      is_sending_started = false;
      sl_app_log("Log sent timeout!\r\n");
  }
}

/***************************************************************************//**
 * Sample timer callback function.
 *
 ******************************************************************************/
static void sli_btmesh_data_log_sample_callback(
            sl_sleeptimer_timer_handle_t *handle,
            void *data)
{
  (void)handle;
  (void)data;

  sl_btmesh_data_log_on_sample_callback();
}

/***************************************************************************//**
 * Periodic Log timer callback function.
 *
 ******************************************************************************/
static void sli_btmesh_data_log_periodic_callback(
            sl_sleeptimer_timer_handle_t *handle,
            void *data)
{
  (void)handle;
  (void)data;
  sl_btmesh_data_log_on_periodic_callback();
}

/***************************************************************************//**
 * Update period requested by client.
 *
 * @param[in] evt Pointer to btmesh Vendor model message.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
static sl_status_t sli_btmesh_data_log_update_period(
                sl_btmesh_evt_vendor_model_receive_t *evt)
{
  sl_status_t st;
  sl_btmesh_data_log_period_t period;
  memcpy((uint8_t *)&period,
         (uint8_t *)evt->payload.data,
         sizeof(sl_btmesh_data_log_period_t));

  st = data_log_save_period(period);

  if(sli_log_started){
    // Re-Start periodic timer
    st = sl_sleeptimer_restart_periodic_timer_ms(
                &sli_data_log_periodic_timer,
                period,
                &sli_btmesh_data_log_periodic_callback,
                NO_CALLBACK_DATA,
                HIGH_PRIORITY,
                NO_FLAGS);

    if(SL_STATUS_OK == st){
        sl_app_log("Period is updated: %d\r\n",
                   period);
    }
  }
  return st;
}

/***************************************************************************//**
 * Update sample rate requested by client.
 *
 * @param[in] evt Pointer to btmesh Vendor model message.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
static sl_status_t sli_btmesh_data_log_update_sample_rate(
                sl_btmesh_evt_vendor_model_receive_t *evt)
{
  sl_status_t st;
  sl_btmesh_data_log_sample_rate_t rate;

  memcpy((uint8_t *)&rate,
         (uint8_t *)evt->payload.data,
         sizeof(sl_btmesh_data_log_sample_rate_t));

  st = data_log_save_sample_rate(rate);

  if(sli_log_started){
    // Re-Start sample timer
    st = sl_sleeptimer_restart_periodic_timer_ms(
                &sli_data_log_sample_timer,
                rate,
                &sli_btmesh_data_log_sample_callback,
                NO_CALLBACK_DATA,
                HIGH_PRIORITY,
                NO_FLAGS);

    if(SL_STATUS_OK == st){
        sl_app_log("Sample rate is updated: %d\r\n", rate);
    }
  }
  return st;
}

/***************************************************************************//**
 * Update threshold requested by client.
 *
 * @param[in] evt Pointer to btmesh Vendor model message.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
static sl_status_t sli_btmesh_data_log_update_threshold(
                sl_btmesh_evt_vendor_model_receive_t *evt)
{
  sl_status_t st;
  sl_btmesh_data_log_threshold_t threshold;

  if(NULL != memcpy((uint8_t *)&threshold,
                    (uint8_t *)evt->payload.data,
                    sizeof(sl_btmesh_data_log_threshold_t)))
  {
    st = data_log_save_threshold(threshold);
    sl_app_log("Threshold is updated: %d\r\n", threshold);
    return st;
  } else { return SL_STATUS_FAIL; }
}

/***************************************************************************//**
 * Get the threshold value.
 *
 * @return returns the current threshold value.
 *
 ******************************************************************************/
sl_btmesh_data_log_threshold_t sl_btmesh_data_log_get_threshold(void)
{
  sl_btmesh_data_log_threshold_t threshold = 0;
  (void)data_log_read_threshold(&threshold);
  return threshold;
}

/***************************************************************************//**
 * Store the period value to the NVM.
 *
 * @param[in] period log period value.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
static sl_status_t data_log_save_period(sl_btmesh_data_log_period_t period)
{
  Ecode_t ec;
  sl_btmesh_data_log_properties_t properties;

  // Read properties
  ec = data_log_read_properties(&properties);

  if(ECODE_NVM3_OK == ec){
      properties.period = period;
      // Write back to NVM
      ec = data_log_write_properties(&properties);
      if(ECODE_NVM3_OK != ec){
          sl_app_log("Failed to save period: 0x%08x\r\n", ec);
          return SL_STATUS_FAIL;
      }
  } else {
      sl_app_log("Failed to save period: 0x%08x\r\n", ec);
      return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Store the sample rate value to the NVM.
 *
 * @param[in] rate sample rate value.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
static sl_status_t data_log_save_sample_rate(
                      sl_btmesh_data_log_sample_rate_t rate)
{
  Ecode_t ec;
  sl_btmesh_data_log_properties_t properties;

  // Read properties
  ec = data_log_read_properties(&properties);

  if(ECODE_NVM3_OK == ec){
      properties.sample_rate = rate;
      // Write back to NVM
      ec = data_log_write_properties(&properties);
      if(ECODE_NVM3_OK != ec){
          sl_app_log("Failed to save sample rate: 0x%08x\r\n", ec);
          return SL_STATUS_FAIL;
      }
  } else {
      sl_app_log("Failed to save sample rate: 0x%08x\r\n", ec);
      return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Store the threshold value to the NVM.
 *
 * @param[in] threshold threshold rate value.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
static sl_status_t data_log_save_threshold(
                      sl_btmesh_data_log_threshold_t threshold)
{
  Ecode_t ec;
  sl_btmesh_data_log_properties_t properties;

  // Read properties
  ec = data_log_read_properties(&properties);

  if(ECODE_NVM3_OK == ec){
      properties.threshold = threshold;
      // Write back to NVM
      ec = data_log_write_properties(&properties);
      if(ECODE_NVM3_OK != ec){
          sl_app_log("Failed to save threshold: 0x%08x\r\n", ec);
          return SL_STATUS_FAIL;
      }
  } else {
      sl_app_log("Failed to save threshold: 0x%08x\r\n", ec);
      return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Read the period value from the NVM.
 *
 * @param[out] period log period value.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
static sl_status_t data_log_read_period(sl_btmesh_data_log_period_t *period)
{
  Ecode_t ec;
  sl_btmesh_data_log_properties_t properties;

  ec = data_log_read_properties(&properties);
  if(ECODE_NVM3_OK == ec){
      *period = properties.period;
      return SL_STATUS_OK;
  } else {
     sl_app_log("Failed to read period: 0x%08x", ec);
  }

  return SL_STATUS_FAIL;
}

/***************************************************************************//**
 * Read the sample rate value from the NVM.
 *
 * @param[out] rate log sample rate value.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
static sl_status_t data_log_read_sample_rate(
                      sl_btmesh_data_log_sample_rate_t *rate)
{
  Ecode_t ec;
  sl_btmesh_data_log_properties_t properties;

  ec = data_log_read_properties(&properties);
  if(ECODE_NVM3_OK == ec){
      *rate = properties.sample_rate;
      return SL_STATUS_OK;
  } else {
     sl_app_log("Failed to read sample rate: 0x%08x", ec);
  }

  return SL_STATUS_FAIL;
}

/***************************************************************************//**
 * Read the threshold value from the NVM.
 *
 * @param[out] threshold threshold value.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
static sl_status_t data_log_read_threshold(
                      sl_btmesh_data_log_threshold_t *threshold)
{
  Ecode_t ec;
  sl_btmesh_data_log_properties_t properties;

  ec = data_log_read_properties(&properties);
  if(ECODE_NVM3_OK == ec){
      *threshold = properties.threshold;
      return SL_STATUS_OK;
  } else {
     sl_app_log("Failed to read threshold: 0x%08x", ec);
  }

  return SL_STATUS_FAIL;
}

/***************************************************************************//**
 * Read the properties structure from the NVM.
 *
 * @param[out] properties pointer to data log properties.
 *
 * @return ECODE_NVM3_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
__STATIC_INLINE Ecode_t data_log_read_properties(
                      sl_btmesh_data_log_properties_t *properties)
{
  return nvm3_readData(nvm3_defaultHandle,
                       SLI_BTMESH_NVM3_DATA_LOG_PROP_KEY,
                       (sl_btmesh_data_log_properties_t *)properties,
                       sizeof(sl_btmesh_data_log_properties_t));
}

/***************************************************************************//**
 * Write the properties structure from the NVM.
 *
 * @param[in] properties pointer to data log properties.
 *
 * @return ECODE_NVM3_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
__STATIC_INLINE Ecode_t data_log_write_properties(
                      sl_btmesh_data_log_properties_t *properties)
{
  return nvm3_writeData(nvm3_defaultHandle,
                       SLI_BTMESH_NVM3_DATA_LOG_PROP_KEY,
                       (sl_btmesh_data_log_properties_t *)properties,
                       sizeof(sl_btmesh_data_log_properties_t));
}

/***************************************************************************//**
 * Check the Data Log object valid or not
 *
 * @return returns the valid status.
 *                 - true if the object key is valid
 *                 - false if the object key is invalid
 *
 ******************************************************************************/
static bool is_storage_valid(void)
{
  uint32_t type;
  size_t len;
  Ecode_t ec = nvm3_getObjectInfo(nvm3_defaultHandle,
                          SLI_BTMESH_NVM3_DATA_LOG_PROP_KEY,
                          &type,
                          &len);
  if((ECODE_NVM3_OK == ec) && (NVM3_OBJECTTYPE_DATA == type)){
      return true;
  } else { return false; }
}

/***************************************************************************//**
 * Delete NVM storage.
 *
 ******************************************************************************/
void sl_btmesh_data_log_reset_config(void)
{
  if(is_storage_valid()){
      (void)nvm3_deleteObject(nvm3_defaultHandle,
                              SLI_BTMESH_NVM3_DATA_LOG_PROP_KEY);
  }
}

/***************************************************************************//**
 * Weak implementation of Callbacks.
 *
 ******************************************************************************/
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

SL_WEAK void sl_btmesh_data_log_on_periodic_callback(void)
{

}

#ifdef __cplusplus
}
#endif
