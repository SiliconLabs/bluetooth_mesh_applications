/***************************************************************************//**
 * @file app_sensor_client.c
 * @brief Application Sensor client module
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
#include "em_common.h"
#include "sl_status.h"

#include "sl_bt_api.h"
#include "sl_btmesh_api.h"
#include "sl_btmesh_dcd.h"

#include "app_assert.h"

#include "app_sensor_client.h"
#include "app_display.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_APP_LOG_PRESENT
#include "app_log.h"
#endif // SL_CATALOG_APP_LOG_PRESENT

// Warning! The app_btmesh_util shall be included after the component configuration
// header file in order to provide the component specific logging macro.
#include "app_btmesh_util.h"

/***************************************************************************//**
 * @addtogroup SensorClient
 * @{
 ******************************************************************************/

/// Number of supported properties
#define PROPERTIES_NUMBER       3
/// Parameter ignored for publishing
#define IGNORED                 0
/// No flags used for message
#define NO_FLAGS                0
/// The size of descriptor is 8 bytes
#define SIZE_OF_DESCRIPTOR      8
/// Size of property ID in bytes
#define PROPERTY_ID_SIZE        2
/// Size of property header in bytes
#define PROPERTY_HEADER_SIZE    3
/// Sensor index value for not registered devices
#define SENSOR_INDEX_NOT_FOUND  0xFF

#define SL_BTMESH_SENSOR_CLIENT_DISPLAYED_SENSORS_CFG_VAL   (5)

typedef struct {
  uint16_t address_table[SL_BTMESH_SENSOR_CLIENT_DISPLAYED_SENSORS_CFG_VAL];
  uint16_t people_count[SL_BTMESH_SENSOR_CLIENT_DISPLAYED_SENSORS_CFG_VAL];
  uint8_t count;
} mesh_registered_device_properties_address_t;

static mesh_registered_device_properties_address_t registered_devices = {
  .count = 0,
};

// Address zero is used in sensor client commands to indicate that
// the message should be published
static const uint16_t PUBLISH_ADDRESS = 0x0000;

// Sum value of each room's occupancy
static count16_t sum_people_count;

static void mesh_sensor_client_init(void);
static void mesh_handle_sensor_client_status(sl_btmesh_evt_sensor_client_status_t *evt);
static uint8_t mesh_get_sensor_index(mesh_registered_device_properties_address_t* property,
                                     uint16_t address);
static bool mesh_address_already_exists(mesh_registered_device_properties_address_t* property,
                                        uint16_t address);
static uint16_t mesh_calculate_sum_people_count(mesh_registered_device_properties_address_t *property);

// -----------------------------------------------------------------------------
// Sensor Model Callbacks

SL_WEAK void sl_btmesh_sensor_client_on_new_people_count_data(uint8_t sensor_idx,
                                                              uint16_t address,
                                                              sl_btmesh_sensor_client_data_status_t status,
                                                              count16_t people_count)
{
  (void) sensor_idx;
  (void) address;
  (void) status;
  (void) people_count;
}

/*******************************************************************************
 * Publishing of sensor client get request for currently displayed property id.
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_client_get_sensor_data(mesh_device_properties_t property)
{
  sl_status_t sc;

  sc = sl_btmesh_sensor_client_get(PUBLISH_ADDRESS,
                                   BTMESH_SENSOR_CLIENT_MAIN,
                                   IGNORED,
                                   NO_FLAGS,
                                   property);

  if (SL_STATUS_OK == sc) {
    log_info("Get Sensor Data from property ID %4.4x started\r\n", property);
  } else {
    log_btmesh_status_f(sc,
                        "Get Sensor Data from property ID %4.4x failed\r\n",
                        property);
  }
  return sc;
}

/*******************************************************************************
 * Get the current sum value of each room's occupancy.
 ******************************************************************************/
count16_t sl_btmesh_get_sum_people_count(void)
{
  return sum_people_count;
}

/*******************************************************************************
 * Handle Sensor Client events.
 ******************************************************************************/
void sl_btmesh_handle_sensor_client_on_event(sl_btmesh_msg_t *evt)
{
  if (NULL == evt) {
    return;
  }

  // Handle events
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_initialized_id:
      if (evt->data.evt_node_initialized.provisioned) {
        mesh_sensor_client_init();
        app_display_show_people_count();
      } else {
        app_display_show_unprovisioned();
      }
      break;

    case sl_btmesh_evt_node_provisioned_id:
      mesh_sensor_client_init();
      app_display_show_people_count();
      break;

    case sl_btmesh_evt_sensor_client_status_id:
      mesh_handle_sensor_client_status(&(evt->data.evt_sensor_client_status));
      break;

    default:
      break;
  }
}

/***************************************************************************//**
 * Handling of sensor client status event.
 *
 * @param[in] evt  Pointer to sensor client status event.
 ******************************************************************************/
static void mesh_handle_sensor_client_status(sl_btmesh_evt_sensor_client_status_t *evt)
{
  uint8_t *sensor_data = evt->sensor_data.data;
  uint8_t data_len = evt->sensor_data.len;
  uint8_t pos = 0;

  while (pos < data_len) {
    if (data_len - pos > PROPERTY_ID_SIZE) {
      mesh_device_properties_t property_id = (mesh_device_properties_t)(sensor_data[pos]
                                                                        + (sensor_data[pos + 1] << 8));
      uint8_t property_len = sensor_data[pos + PROPERTY_ID_SIZE];
      uint8_t *property_data = NULL;
      sl_btmesh_sensor_client_data_status_t status;
      uint16_t address;
      uint8_t sensor_idx;
      uint8_t number_of_devices = registered_devices.count;

      if (property_len && (data_len - pos > PROPERTY_HEADER_SIZE)) {
        property_data = &sensor_data[pos + PROPERTY_HEADER_SIZE];
      }

      address = evt->server_address;
      // Check if the mesh address already exists or not.
      // if not, store it to address table
      if (!mesh_address_already_exists(&registered_devices, address)) {
        registered_devices.address_table[number_of_devices] = address;
        registered_devices.count = number_of_devices + 1;
        // Show log
        sl_btmesh_sensor_client_on_new_device_found(property_id, address);
      }

      sensor_idx = mesh_get_sensor_index(&registered_devices, address);
      status = SL_BTMESH_SENSOR_CLIENT_DATA_NOT_AVAILABLE;

        if (PEOPLE_COUNT == property_id) {
          count16_t people_count = SL_BTMESH_SENSOR_CLIENT_PEOPLE_COUNT_UNKNOWN;

          if (property_len == 2) {
            mesh_device_property_t new_property = mesh_sensor_data_from_buf(PEOPLE_COUNT,
                                                                            property_data);
            people_count = new_property.count16;

            if (people_count == SL_BTMESH_SENSOR_CLIENT_PEOPLE_COUNT_UNKNOWN) {
              status = SL_BTMESH_SENSOR_CLIENT_DATA_UNKNOWN;
            } else {
              status = SL_BTMESH_SENSOR_CLIENT_DATA_VALID;
            }
          } else {
            status = SL_BTMESH_SENSOR_CLIENT_DATA_NOT_AVAILABLE;
          }
          // Print log
          sl_btmesh_sensor_client_on_new_people_count_data(sensor_idx,
                                                           address,
                                                           status,
                                                           people_count);
          // Update to oled and component
          // Ignore if data is invalid or data is comming from itself
          if ((SL_BTMESH_SENSOR_CLIENT_DATA_VALID == status)
              && (evt->client_address != address)) {
            registered_devices.people_count[sensor_idx] = people_count;
            sum_people_count = mesh_calculate_sum_people_count(&registered_devices);
            app_display_update_people_count(sum_people_count, number_of_devices);
          }
        }
      pos += PROPERTY_HEADER_SIZE + property_len;
    } else {
      pos = data_len;
    }
  }
}

/***************************************************************************//**
 * Check if the mesh address already exists or not.
 *
 * @param[in] property Pointer to registered devices' properties
 * @param[in] address  Mesh address to check
 *
 * @return             true:  The address exists
 *                     false: The address doesn't exist
 ******************************************************************************/
static bool mesh_address_already_exists(mesh_registered_device_properties_address_t *property,
                                        uint16_t address)
{
  bool address_exists = false;

  if (property != NULL) {
    for (int i = 0; i < SL_BTMESH_SENSOR_CLIENT_DISPLAYED_SENSORS_CFG_VAL; i++) {
      if (address == property->address_table[i]) {
        address_exists = true;
        break;
      }
    }
  }
  return address_exists;
}

/***************************************************************************//**
 * Calculates current sum value of each room's occupancy.
 *
 * @return  Current sum of the people count
 ******************************************************************************/
static uint16_t mesh_calculate_sum_people_count(mesh_registered_device_properties_address_t *property)
{
  uint16_t sum = 0;

  if (property != NULL) {
    for (int i = 0; i < property->count; i++) {
      sum += property->people_count[i];
    }
  }
  return sum;
}

/***************************************************************************//**
 * Gets the sensor index.
 *
 * @param[in] property Pointer to registered devices' properties
 * @param[in] address  Mesh address of the sensor
 *
 * @return             Index of the sensor
 ******************************************************************************/
static uint8_t mesh_get_sensor_index(mesh_registered_device_properties_address_t *property,
                                     uint16_t address)
{
  uint8_t sensor_index = SENSOR_INDEX_NOT_FOUND;
  if (property != NULL) {
    for (int i = 0; i < SL_BTMESH_SENSOR_CLIENT_DISPLAYED_SENSORS_CFG_VAL; i++) {
      if (address == property->address_table[i]) {
        sensor_index = i;
        break;
      }
    }
  }
  return sensor_index;
}

/***************************************************************************//**
 * Initializes sensor client
 ******************************************************************************/
static void mesh_sensor_client_init(void)
{
  sl_status_t sc = sl_btmesh_sensor_client_init();

  app_assert_status_f(sc, "Failed to initialize sensor client\n");
}

/** @} (end addtogroup SensorClient) */
