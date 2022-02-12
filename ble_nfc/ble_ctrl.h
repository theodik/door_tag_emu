#pragma once
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"

#define DEVICE_UUID     0x9000
#define CTRL_CHAR_UUID  0x9001

/**@brief Macro for defining a ble_ctrl instance.
 *
 * @param   _name  Name of the instance.
 * @hideinitializer
 */
#define BLE_CTRL_DEF(_name)                         \
    static ble_ctrl_t _name;                        \
    NRF_SDH_BLE_OBSERVER(_name ## _obs,             \
                         BLE_BAS_BLE_OBSERVER_PRIO, \
                         ble_ctrl_on_ble_evt,       \
                         &_name)

// Forward declaration of the ble_bas_t type.
typedef struct ble_ctrl_s ble_ctrl_t;

typedef void (*ble_ctrl_on_change_t)(uint8_t const * uid, size_t uid_length);

struct ble_ctrl_s {
    uint16_t conn_handle;
    uint16_t service_handle; /**< Handle of Box Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t char_control_handle;

    ble_ctrl_on_change_t on_change;
};

ret_code_t ble_ctrl_init(ble_ctrl_t * p_ctrl, ble_ctrl_on_change_t callback);

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the Box Service.
 *
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 * @param[in]   p_context   Box Service structure.
 */
void ble_ctrl_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

uint32_t ble_ctrl_set_uid(ble_ctrl_t const * p_ctrl, uint8_t * uid, size_t uid_len);
