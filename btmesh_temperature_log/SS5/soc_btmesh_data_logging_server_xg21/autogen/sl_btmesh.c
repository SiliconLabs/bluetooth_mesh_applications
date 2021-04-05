

#include <em_common.h>
#include "sl_btmesh.h"
#include "sl_btmesh_provisioning_decorator.h"

/** @brief Table of used BGAPI classes */
static const struct sli_bgapi_class * const btmesh_class_table[] =
{
  SL_BTMESH_BGAPI_CLASS(generic_server),
  SL_BTMESH_BGAPI_CLASS(health_server),
  SL_BTMESH_BGAPI_CLASS(proxy),
  SL_BTMESH_BGAPI_CLASS(proxy_server),
  SL_BTMESH_BGAPI_CLASS(vendor_model),
  SL_BTMESH_BGAPI_CLASS(node),
  NULL
};

void sl_btmesh_init(void)
{
  sl_btmesh_init_classes(btmesh_class_table);
}

SL_WEAK void sl_btmesh_on_event(sl_btmesh_msg_t* evt)
{
  (void)(evt);
}

void sl_btmesh_process_event(sl_btmesh_msg_t *evt)
{
  sl_btmesh_handle_provisioning_decorator_event(evt);
  sl_btmesh_on_event(evt);
}

SL_WEAK bool sl_btmesh_can_process_event(uint32_t len)
{
  (void)(len);
  return true;
}

void sl_btmesh_step(void)
{
  sl_btmesh_msg_t evt;

  // check if application can process a new event.
  if (!sl_btmesh_can_process_event(SL_BGAPI_MSG_HEADER_LEN + SL_BGAPI_MAX_PAYLOAD_SIZE)) {
    return;
  }

  // Pop (non-blocking) a Bluetooth stack event from event queue.
  sl_status_t status = sl_btmesh_pop_event(&evt);
  if(status != SL_STATUS_OK){
    return;
  }
  sl_btmesh_process_event(&evt);
}
