#include <app_error.h>
#include "ble_ctrl.h"

#include "sdk_common.h"
#define NRF_LOG_MODULE_NAME ble_ctrl
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

ret_code_t ble_ctrl_init(ble_ctrl_t * p_ctrl, ble_ctrl_on_change_t callback) {
    if (p_ctrl == NULL) {
        return NRF_ERROR_NULL;
    }

    ret_code_t err_code;
    ble_uuid_t ble_uuid;

    p_ctrl->conn_handle = BLE_CONN_HANDLE_INVALID;
    p_ctrl->on_change = callback;

    // Add service
    BLE_UUID_BLE_ASSIGN(ble_uuid, DEVICE_UUID);
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_ctrl->service_handle);
    VERIFY_SUCCESS(err_code);

    ble_add_char_params_t add_char_params = {};

    add_char_params.uuid = CTRL_CHAR_UUID;
    add_char_params.max_len = 10;
    add_char_params.char_props.write = 1;
    add_char_params.char_props.read = 1;
    add_char_params.write_access = SEC_OPEN;
    add_char_params.read_access = SEC_OPEN;

    err_code = characteristic_add(p_ctrl->service_handle, &add_char_params, &p_ctrl->char_control_handle);
    return err_code;
}

void ble_ctrl_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context) {
    if ((p_context == NULL) || (p_ble_evt == NULL)) {
        return;
    }

    ble_ctrl_t * p_ctrl = (ble_ctrl_t *)p_context;
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    switch (p_ble_evt->header.evt_id) {
        case BLE_GATTS_EVT_WRITE: {
            uint8_t const * uid = p_evt_write->data;
            size_t uid_length = p_evt_write->len;

            p_ctrl->on_change(uid, uid_length);
        } break;

        case BLE_GAP_EVT_CONNECTED:
            p_ctrl->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            p_ctrl->conn_handle = BLE_CONN_HANDLE_INVALID;
            break;

        default:
            // No implementation needed.
            break;
    }
}

uint32_t ble_ctrl_set_uid(ble_ctrl_t const * p_ctrl, uint8_t * uid, size_t uid_len) {
    uint32_t err_code;
    ble_gatts_value_t value = {
        .len = uid_len,
        .offset = 0,
        .p_value = uid
    };

    err_code = sd_ble_gatts_value_set(p_ctrl->conn_handle, p_ctrl->char_control_handle.value_handle, &value);

    return err_code;
}
