/***************************************************************************//**
* @file sl_btmesh_data_logging_client.c
* @brief BT Mesh Data Logging Client Instances
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
#include "sl_btmesh_data_logging_client.h"
#include "em_common.h"

/// Data logging structure
static sl_data_log_t *sli_data_log_ptr = NULL;

/// Receive handler
static sl_status_t sli_btmesh_data_log_receive_handler(
              sl_btmesh_msg_t *evt);

/// Receive timeout callback
static void sli_btmesh_data_log_recv_timeout_callback(
            sl_sleeptimer_timer_handle_t *handle,
            void *data);

/// Response timer callback
static void sli_btmesh_data_log_resp_timer_callback(
            sl_sleeptimer_timer_handle_t *handle,
            void *data);

/// Counter for the log receive
static uint8_t sli_recv_status;

/// Timer for receive timeout
static sl_sleeptimer_timer_handle_t sli_data_log_timeout_timer;

/// Timer for send response
static sl_sleeptimer_timer_handle_t sli_data_log_resp_timer;

/// Used for send response
static uint16_t   sli_elem_index;
static uint16_t   sli_source_address;
static int8_t     sli_va_index;
static uint16_t   sli_appkey_index;
static uint8_t    sli_nonrelayed;

sl_status_t sl_btmesh_data_log_client_init(sl_data_log_t *data_log_ptr)
{
  sl_status_t st;
  if(NULL != data_log_ptr){
    sli_data_log_ptr = data_log_ptr;
    st = sl_btmesh_vendor_model_init(SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL,
                                     SL_BTMESH_VENDOR_ID,
                                     SL_BTMESH_DATA_LOG_MODEL_CLIENT_ID,
                                     SL_BTMESH_MODEL_PUBLISH,
                                     SL_BTMESH_DATA_LOG_OPCODE_LENGTH,
                                     sl_btmesh_data_log_opcodes);

    if(SL_STATUS_OK == st){
      // Clear log
      sli_data_log_ptr->index = 0;
      if(NULL != sli_data_log_ptr->data){
        memset(sli_data_log_ptr->data,
               SL_BTMESH_DATA_LOG_CLEAR_VAL,
               sizeof(sl_data_log_data_t)*SL_BTMESH_DATA_LOG_BUFF_SIZE_CFG_VAL);
      } else { return SL_STATUS_NULL_POINTER; }
    } else {
        return st;
    }

    // Reset receive status
    sli_recv_status = SL_BTMESH_DATA_LOG_IDLE;
  } else { st = SL_STATUS_NULL_POINTER; }

  return st;
}

sl_status_t sl_btmesh_data_log_client_deinit(void)
{
  sli_recv_status = SL_BTMESH_DATA_LOG_IDLE;
  // Stop timers
  sl_sleeptimer_stop_timer(&sli_data_log_timeout_timer);
  sl_sleeptimer_stop_timer(&sli_data_log_resp_timer);
  return sl_btmesh_vendor_model_deinit(SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL,
                                       SL_BTMESH_VENDOR_ID,
                                       SL_BTMESH_DATA_LOG_MODEL_CLIENT_ID);
}

static sl_status_t sli_btmesh_data_log_receive_handler(
              sl_btmesh_msg_t *evt)
{
  sl_status_t st;

  sl_btmesh_evt_vendor_model_receive_t *log_evt =
          &(evt->data.evt_vendor_model_receive);

  // Receive length
  uint8_t recv_len = log_evt->payload.len - SL_BTMESH_BYTE_FLAG_LEN;
  // Point to received data
  sl_data_frame_t *frame = (sl_data_frame_t *)&(
      log_evt->payload.data[SL_BTMESH_BYTE_FLAG_POS+SL_BTMESH_BYTE_FLAG_LEN]);

  if((SL_BTMESH_DATA_LOG_IDLE == sli_recv_status)
      || (SL_BTMESH_DATA_LOG_COMPLETE == sli_recv_status)){
      sli_recv_status = SL_BTMESH_DATA_LOG_BUSY;
      // Start timer
      st = sl_sleeptimer_start_timer_ms(&sli_data_log_timeout_timer,
                                  SL_BTMESH_DATA_LOG_TIMEOUT_MS_CFG_VAL,
                                  &sli_btmesh_data_log_recv_timeout_callback,
                                  NO_CALLBACK_DATA,
                                  HIGH_PRIORITY,
                                  NO_FLAGS);
      if(SL_STATUS_OK != st){
          return st;
      }
  }
  sl_data_log_index_t index = sli_data_log_ptr->index
                              + recv_len/sizeof(sl_data_log_data_t);
  if(index < SL_BTMESH_DATA_LOG_BUFF_SIZE_CFG_VAL){
      // Copy received data
      if(NULL == (uint8_t *)memcpy(((uint8_t *)&sli_data_log_ptr->data[index]),
             (uint8_t *)frame,
             recv_len))
      {
          return SL_STATUS_ALLOCATION_FAILED;
      }
      // Update new index
      sli_data_log_ptr->index = index;
  } else { return SL_STATUS_FULL; }

  // Trigger timer to send response massage
  sli_source_address = log_evt->source_address;
  sli_va_index = log_evt->va_index;
  sli_appkey_index = log_evt->appkey_index;
  sli_elem_index = log_evt->elem_index;
  sli_nonrelayed = log_evt->nonrelayed;

  st = sl_sleeptimer_start_timer_ms(&sli_data_log_resp_timer,
                                  SL_BTMESH_DATA_LOG_RESP_MS_CFG_VAL,
                                  &sli_btmesh_data_log_resp_timer_callback,
                                  NO_CALLBACK_DATA,
                                  HIGH_PRIORITY,
                                  NO_FLAGS);

  if(SL_STATUS_OK != st){
      return st;
  }

  uint8_t last_seg = log_evt->payload.data[SL_BTMESH_BYTE_FLAG_POS];
  if(SL_BTMESH_DATA_LOG_LAST == last_seg){
      sli_recv_status = SL_BTMESH_DATA_LOG_COMPLETE;
      // Stop timeout timer
      st = sl_sleeptimer_stop_timer(&sli_data_log_timeout_timer);
      // Execute complete callback
      sl_btmesh_data_log_client_recv_complete_callback();
  } else { st = SL_STATUS_OK; }

  return st;
}

sl_status_t sl_btmesh_data_log_on_client_receive_event(
        sl_btmesh_msg_t *evt)
{
  sl_btmesh_evt_vendor_model_receive_t *log_evt =
          &(evt->data.evt_vendor_model_receive);

  if((SL_BTMESH_VENDOR_ID == log_evt->vendor_id)
      &&(SL_BTMESH_DATA_LOG_MODEL_CLIENT_ID == log_evt->model_id)
      &&(SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL == log_evt->elem_index)
      && (SL_BTMESH_DATA_LOG_MESSAGE_STATUS_ID == log_evt->opcode)){
      return sli_btmesh_data_log_receive_handler(evt);
  }

  return SL_STATUS_OK;
}

sl_status_t sl_btmesh_data_log_client_set_period(
                sl_btmesh_data_log_period_t *period)
{
  if (SL_BTMESH_DATA_LOG_PROP_LEN < sizeof(sl_btmesh_data_log_period_t)){
      return SL_STATUS_INVALID_RANGE;
  }

  uint8_t tmp_buff[SL_BTMESH_DATA_LOG_PROP_LEN];
  uint8_t *ptr;

  ptr = (uint8_t *)period;
  memcpy((uint8_t *)tmp_buff,
         (uint8_t *)ptr,
         sizeof(sl_btmesh_data_log_period_t));

  sl_status_t st = sl_btmesh_vendor_model_set_publication(
                          SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL,
                          SL_BTMESH_VENDOR_ID,
                          SL_BTMESH_DATA_LOG_MODEL_CLIENT_ID,
                          SL_BTMESH_DATA_LOG_MESSAGE_PERIOD_ID,
                          SL_BTMESH_SEGMENT_FINAL,
                          sizeof(sl_btmesh_data_log_period_t),
                          (const uint8_t *)tmp_buff);

  // Send period
  if(SL_STATUS_OK == st){
    st = sl_btmesh_vendor_model_publish(
                          SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL,
                          SL_BTMESH_VENDOR_ID,
                          SL_BTMESH_DATA_LOG_MODEL_CLIENT_ID);
  }

  return st;
}

sl_status_t sl_btmesh_data_log_client_set_sample_rate(
                sl_btmesh_data_log_sample_rate_t *rate)
{
  if (SL_BTMESH_DATA_LOG_PROP_LEN < sizeof(sl_btmesh_data_log_sample_rate_t)){
      return SL_STATUS_INVALID_RANGE;
  }

  uint8_t tmp_buff[SL_BTMESH_DATA_LOG_PROP_LEN];
  uint8_t *ptr;

  ptr = (uint8_t *)rate;
  memcpy((uint8_t *)tmp_buff,
         (uint8_t *)ptr,
         sizeof(sl_btmesh_data_log_sample_rate_t));

  sl_status_t st = sl_btmesh_vendor_model_set_publication(
                          SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL,
                          SL_BTMESH_VENDOR_ID,
                          SL_BTMESH_DATA_LOG_MODEL_CLIENT_ID,
                          SL_BTMESH_DATA_LOG_MESSAGE_SAMPLE_RATE_ID,
                          SL_BTMESH_SEGMENT_FINAL,
                          sizeof(sl_btmesh_data_log_sample_rate_t),
                          (const uint8_t *)tmp_buff);

  // Send period
  if(SL_STATUS_OK == st){
    st = sl_btmesh_vendor_model_publish(
                          SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL,
                          SL_BTMESH_VENDOR_ID,
                          SL_BTMESH_DATA_LOG_MODEL_CLIENT_ID);
  }

  return st;
}

sl_status_t sl_btmesh_data_log_client_set_raw(
                sl_btmesh_data_log_raw_t *raw)
{
  if (SL_BTMESH_DATA_LOG_PROP_LEN < sizeof(sl_btmesh_data_log_raw_t)){
      return SL_STATUS_INVALID_RANGE;
  }

  uint8_t tmp_buff[SL_BTMESH_DATA_LOG_PROP_LEN];
  uint8_t *ptr;

  ptr = (uint8_t *)raw;
  memcpy((uint8_t *)tmp_buff,
         (uint8_t *)ptr,
         sizeof(sl_btmesh_data_log_raw_t));

  sl_status_t st = sl_btmesh_vendor_model_set_publication(
                          SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL,
                          SL_BTMESH_VENDOR_ID,
                          SL_BTMESH_DATA_LOG_MODEL_CLIENT_ID,
                          SL_BTMESH_DATA_LOG_MESSAGE_RAW_ID,
                          SL_BTMESH_SEGMENT_FINAL,
                          sizeof(sl_btmesh_data_log_raw_t),
                          (const uint8_t *)tmp_buff);

  // Send period
  if(SL_STATUS_OK == st){
    st = sl_btmesh_vendor_model_publish(
                          SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL,
                          SL_BTMESH_VENDOR_ID,
                          SL_BTMESH_DATA_LOG_MODEL_CLIENT_ID);
  }

  return st;
}

static void sli_btmesh_data_log_recv_timeout_callback(
            sl_sleeptimer_timer_handle_t *handle,
            void *data)
{
  (void)handle;
  (void)data;

  // Receive timeout
  if(SL_BTMESH_DATA_LOG_BUSY == sli_recv_status){
      sli_data_log_ptr->index = 0;
      sli_recv_status = SL_BTMESH_DATA_LOG_IDLE;
      sl_app_log("Log receive timeout!\r\n");
  }
}

static void sli_btmesh_data_log_resp_timer_callback(
            sl_sleeptimer_timer_handle_t *handle,
            void *data)
{
  (void)handle;
  (void)data;

  uint8_t tmp_buff[SL_BTMESH_DATA_LOG_PROP_LEN];

  sl_btmesh_vendor_model_send(sli_source_address,
                             sli_va_index,
                             sli_appkey_index,
                             sli_elem_index,
                             SL_BTMESH_VENDOR_ID,
                             SL_BTMESH_DATA_LOG_MODEL_CLIENT_ID,
                             sli_nonrelayed,
                             SL_BTMESH_DATA_LOG_MESSAGE_STATUS_RSP_ID,
                             SL_BTMESH_SEGMENT_FINAL,
                             SL_BTMESH_DATA_LOG_MESSAGE_STATUS_RSP_LEN,
                             (const uint8_t *)tmp_buff);
}

SL_WEAK void sl_btmesh_data_log_client_recv_complete_callback(void)
{

}

uint8_t sl_btmesh_data_log_get_client_state(void)
{
  return sli_recv_status;
}

#ifdef __cplusplus
}
#endif
