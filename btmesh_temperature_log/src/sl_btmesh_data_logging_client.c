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
#include "app_log.h"
#include "sl_btmesh_data_logging_client.h"
#include "em_common.h"

// Reset Log
#define SLI_RESET_LOG \
          sli_data_log_ptr->index = SL_BTMESH_DATA_LOG_RESET_VAL; \
          sli_recv_status = SL_BTMESH_DATA_LOG_IDLE

/// Data logging structure
static sl_data_log_recv_t *sli_data_log_ptr = NULL;

/// Receive handler
static sl_status_t sli_btmesh_data_log_receive_handler(
              sl_btmesh_msg_t *evt);

/// Receive timeout callback
static void sli_btmesh_data_log_recv_timeout_callback(
            sl_sleeptimer_timer_handle_t *handle,
            void *data);

/// Data receive handler
static sl_status_t sli_btmesh_data_log_data_recv_handler(sl_btmesh_msg_t *evt);

/// Counter for the log receive
static uint8_t sli_recv_status;

/// Timer for receive timeout
static sl_sleeptimer_timer_handle_t sli_data_log_timeout_timer;

/// Timer for send response
static sl_sleeptimer_timer_handle_t sli_data_log_resp_timer;

/// Received data ID
static uint8_t sli_recv_count;

/// Used to store the received data
static sl_data_log_data_t sli_data_received;

/***************************************************************************//**
 * Initialize the data log client.
 *
 * @param[in] data_log_ptr Pointer to Log data instance.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_client_init(sl_data_log_recv_t *data_log_ptr)
{
  sl_status_t st;
  sli_recv_count = SL_BTMESH_RECV_ID_INIT_VAL;
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
      sli_data_log_ptr->index = SL_BTMESH_DATA_LOG_RESET_VAL;
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

/***************************************************************************//**
 * De-Initialize the data log client.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
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

/***************************************************************************//**
 * Handle receiving packages using vendor model.
 *
 * @param[in] evt Pointer to btmesh message.
 *
 * Start of receiving handler will trigger a timeout timer. Within the timeout,
 * if packages received completely, a callback function will be call. Otherwise,
 * a timeout occurs and the Log will be reset.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
static sl_status_t sli_btmesh_data_log_receive_handler(sl_btmesh_msg_t *evt)
{
  sl_status_t st;
  sl_btmesh_evt_vendor_model_receive_t *log_evt =
          &(evt->data.evt_vendor_model_receive);

  // Receive length
  uint8_t recv_len = log_evt->payload.len - SL_BTMESH_DATA_HEADER_LEN;
  // Point to received data
  sl_data_frame_t *frame = (sl_data_frame_t *)(log_evt->payload.data);
  uint8_t *recv_data =
      (uint8_t *)&log_evt->payload.data[SL_BTMESH_DATA_HEADER_LEN];

  sl_data_log_index_t index = sli_data_log_ptr->index;
  if(index < SL_BTMESH_DATA_LOG_BUFF_SIZE_CFG_VAL){
      // Copy received data
      if(NULL == (uint8_t *)memcpy(((uint8_t *)&sli_data_log_ptr->data[index]),
             recv_data,
             recv_len))
      {
          return SL_STATUS_ALLOCATION_FAILED;
      }
      // Update new index
      sli_data_log_ptr->index = index + recv_len/sizeof(sl_data_log_data_t);
  } else { return SL_STATUS_FULL; }

  if(SL_BTMESH_DATA_LOG_LAST == frame->header.last){
      // Stop timeout timer
      bool timer_running;
      st = sl_sleeptimer_is_timer_running(&sli_data_log_timeout_timer,
                                         &timer_running);
      if(SL_STATUS_OK == st){
        if(timer_running){
            st = sl_sleeptimer_stop_timer(&sli_data_log_timeout_timer);
        }
      }
      if(sli_recv_count != frame->header.count){
          sli_data_log_ptr->source_addr = log_evt->source_address;
          sli_data_log_ptr->dest_addr = log_evt->destination_address;
          sli_recv_status = SL_BTMESH_DATA_LOG_COMPLETE;
          sli_recv_count = frame->header.count;
          // Execute complete callback
          sl_btmesh_data_log_client_recv_complete_callback();
      } else { // Duplicated data received
          sli_data_log_ptr->index = SL_BTMESH_DATA_LOG_RESET_VAL;
          sli_recv_status = SL_BTMESH_DATA_LOG_IDLE;
      }
  } else if((SL_BTMESH_DATA_LOG_IDLE == sli_recv_status)
      || (SL_BTMESH_DATA_LOG_COMPLETE == sli_recv_status)){
      sli_recv_status = SL_BTMESH_DATA_LOG_BUSY;
      // Start timeout timer
      st = sl_sleeptimer_start_timer_ms(&sli_data_log_timeout_timer,
                                  SL_BTMESH_DATA_LOG_TIMEOUT_MS_CFG_VAL,
                                  &sli_btmesh_data_log_recv_timeout_callback,
                                  NO_CALLBACK_DATA,
                                  HIGH_PRIORITY,
                                  NO_FLAGS);
      if(SL_STATUS_OK != st){
          // Error occurs, Reset Log
          SLI_RESET_LOG;
          return st;
      }
  } else { st = SL_STATUS_OK; }

  return st;
}

/***************************************************************************//**
 * Handle receiving data using vendor model.
 *
 * @param[in] evt Pointer to btmesh message.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
static sl_status_t sli_btmesh_data_log_data_recv_handler(sl_btmesh_msg_t *evt)
{
  sl_btmesh_evt_vendor_model_receive_t *log_evt =
          &(evt->data.evt_vendor_model_receive);

  if(NULL != memcpy((uint8_t *)&sli_data_received,
                     log_evt->payload.data,
                     log_evt->payload.len)){
      sl_btmesh_data_log_client_data_recv_callback(&sli_data_received);
      return SL_STATUS_OK;
  } else { return SL_STATUS_FAIL; }
}

/***************************************************************************//**
 * Handle event of the Log client.
 *
 * @param[in] evt Pointer to btmesh message.
 *
 * Handle event of the Vendor model. If there's valid opcode received then
 * receiving handler will be called.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 *  @note This function must be executed in sl_btmesh_on_event for the
 *  sl_btmesh_evt_vendor_model_receive_id event.
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_on_client_receive_event(
        sl_btmesh_msg_t *evt)
{
  sl_status_t st;
  sl_btmesh_evt_vendor_model_receive_t *log_evt =
          &(evt->data.evt_vendor_model_receive);

  if((SL_BTMESH_VENDOR_ID == log_evt->vendor_id)
      &&(SL_BTMESH_DATA_LOG_MODEL_CLIENT_ID == log_evt->model_id)
      &&(SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL == log_evt->elem_index)){
    switch(log_evt->opcode){
      case SL_BTMESH_DATA_LOG_MESSAGE_STATUS_ID:
        // Handle received data
        st = sli_btmesh_data_log_receive_handler(evt);
        #if defined(SL_BTMESH_DATA_LOG_RSP_ENABLE)
        if(SL_STATUS_OK == st){
          uint8_t tmp_buff[SL_BTMESH_DATA_LOG_PROP_LEN];
          // Send response
          st = sl_btmesh_vendor_model_send(log_evt->source_address,
                                     log_evt->va_index,
                                     log_evt->appkey_index,
                                     log_evt->elem_index,
                                     SL_BTMESH_VENDOR_ID,
                                     SL_BTMESH_DATA_LOG_MODEL_CLIENT_ID,
                                     log_evt->nonrelayed,
                                     SL_BTMESH_DATA_LOG_MESSAGE_STATUS_RSP_ID,
                                     SL_BTMESH_SEGMENT_FINAL,
                                     SL_BTMESH_DATA_LOG_MESSAGE_STATUS_RSP_LEN,
                                     (const uint8_t *)tmp_buff);
          if(SL_STATUS_OK == st){
              app_log("Sent Status response\r\n");
          }
        }
        #endif // SL_BTMESH_DATA_LOG_RSP_ENABLE
        break;
      case SL_BTMESH_DATA_LOG_MESSAGE_TEMP_ID:
        st = sli_btmesh_data_log_data_recv_handler(evt);
        break;
      default: st = SL_STATUS_OK;
    }
  } else { st = SL_STATUS_OK; }

  return st;
}

/***************************************************************************//**
 * Send request to update Log period to server.
 *
 * @param[in] period Pointer to period data type instance.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
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

/***************************************************************************//**
 * Send request to update sample rate to server.
 *
 * @param[in] rate Pointer to sample rate data type instance.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
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

/***************************************************************************//**
 * Send request to update sample threshold to server.
 *
 * @param[in] threshold Pointer to threshold data type instance.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_client_set_threshold(
                sl_btmesh_data_log_threshold_t *threshold)
{
  if (SL_BTMESH_DATA_LOG_PROP_LEN < sizeof(sl_btmesh_data_log_threshold_t)){
      return SL_STATUS_INVALID_RANGE;
  }

  uint8_t tmp_buff[SL_BTMESH_DATA_LOG_PROP_LEN];
  uint8_t *ptr;

  ptr = (uint8_t *)threshold;
  memcpy((uint8_t *)tmp_buff,
         (uint8_t *)ptr,
         sizeof(sl_btmesh_data_log_threshold_t));

  sl_status_t st = sl_btmesh_vendor_model_set_publication(
                          SL_BTMESH_DATA_LOG_ELEMENT_CFG_VAL,
                          SL_BTMESH_VENDOR_ID,
                          SL_BTMESH_DATA_LOG_MODEL_CLIENT_ID,
                          SL_BTMESH_DATA_LOG_MESSAGE_THRESHOLD_ID,
                          SL_BTMESH_SEGMENT_FINAL,
                          sizeof(sl_btmesh_data_log_threshold_t),
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

/***************************************************************************//**
 * Timeout timer callback function.
 *
 ******************************************************************************/
static void sli_btmesh_data_log_recv_timeout_callback(
            sl_sleeptimer_timer_handle_t *handle,
            void *data)
{
  (void)handle;
  (void)data;

  // Receive timeout
  if(SL_BTMESH_DATA_LOG_BUSY == sli_recv_status){
      sli_data_log_ptr->index = SL_BTMESH_DATA_LOG_RESET_VAL;
      sli_recv_status = SL_BTMESH_DATA_LOG_IDLE;
      app_log("Log receive timeout!\r\n");
  }
}

/***************************************************************************//**
 * Reset the Log.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_btmesh_data_log_client_reset_log(void)
{
  if(SL_BTMESH_DATA_LOG_BUSY != sli_recv_status){
      if(NULL != sli_data_log_ptr){
          SLI_RESET_LOG;
          return SL_STATUS_OK;
      } else { return SL_STATUS_NOT_INITIALIZED; }
  }

  return SL_STATUS_BUSY;
}

/***************************************************************************//**
 * Log receive complete callback function.
 *
 ******************************************************************************/
SL_WEAK void sl_btmesh_data_log_client_recv_complete_callback(void)
{

}

/***************************************************************************//**
 * Data receive complete callback function.
 *
 ******************************************************************************/
SL_WEAK void sl_btmesh_data_log_client_data_recv_callback(
                                    sl_data_log_data_t *data)
{
  (void)data;
}

/***************************************************************************//**
 * Get the current status of the Log.
 *
 * @return Return the current status of the Log.
 *  - SL_BTMESH_DATA_LOG_IDLE if Log is idle.
 *  - SL_BTMESH_DATA_LOG_BUSY if Log is receiving.
 *  - SL_BTMESH_DATA_LOG_COMPLETE if Log is received.
 *
 ******************************************************************************/
uint8_t sl_btmesh_data_log_get_client_state(void)
{
  return sli_recv_status;
}

#ifdef __cplusplus
}
#endif
