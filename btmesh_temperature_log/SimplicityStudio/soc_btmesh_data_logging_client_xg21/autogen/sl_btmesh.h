#ifndef SL_BTMESH_H
#define SL_BTMESH_H

#include <stdbool.h>
#include "sl_power_manager.h"
#include "sl_btmesh_config.h"
#include "sl_btmesh_api.h"
#include "sl_btmesh_stack_init.h"
#include "sl_bt_api.h"
#include "sl_btmesh_bgapi.h"

#define SL_BTMESH_COMPONENT_ADVERTISERS (3 + SL_BTMESH_CONFIG_MAX_NETKEYS)

#define SL_BTMESH_FEATURE_BITMASK 3
#define SL_BTMESH_CONFIG_MAX_PROV_BEARERS 2

// Initialize Bluetooth core functionality
void sl_btmesh_init(void);

// Polls bluetooth stack for an event and processes it
void sl_btmesh_step(void);

// Processes a single bluetooth mesh event
void sl_btmesh_process_event(sl_btmesh_msg_t *evt);

bool sl_btmesh_can_process_event(uint32_t len);

void sl_btmesh_on_event(sl_btmesh_msg_t* evt);

// Power Manager related functions
bool sli_btmesh_is_ok_to_sleep(void);
sl_power_manager_on_isr_exit_t sli_btmesh_sleep_on_isr_exit(void);

#endif // SL_BTMESH_H
