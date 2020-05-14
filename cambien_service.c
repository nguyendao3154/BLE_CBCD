
#include "sdk_common.h"
#include "cambien_service.h"
#include "ble_srv_common.h"

void ble_cb_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_contex)
{
    switch (p_ble_evt->header.evt_id)
    {

    default:
        // No implementation needeble_cb_init_td.
        break;
    }
}
uint32_t ble_cb_init(ble_cb_t *p_cb)
{
    uint32_t err_code;
    ble_uuid_t ble_uuid;
    ble_add_char_params_t add_char_params;

    // Add service.
    ble_uuid128_t base_uuid = {CB_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_cb->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_cb->uuid_type;
    ble_uuid.uuid = CB_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_cb->service_handle);
    VERIFY_SUCCESS(err_code);

    // Add chuyen dong characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid = CB_UUID_CHUYENDONG_CHAR;
    add_char_params.uuid_type = p_cb->uuid_type;
    add_char_params.init_len = sizeof(uint8_t);
    add_char_params.max_len = sizeof(uint8_t);
    add_char_params.char_props.read = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access = SEC_OPEN;
    add_char_params.cccd_write_access = SEC_OPEN;

    err_code = characteristic_add(p_cb->service_handle,
                                  &add_char_params,
                                  &p_cb->chuyendong_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add tu characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid = Cb_UUID_TU_CHAR;
    add_char_params.uuid_type = p_cb->uuid_type;
    add_char_params.init_len = sizeof(uint8_t);
    add_char_params.max_len = sizeof(uint8_t);
    add_char_params.char_props.read = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access = SEC_OPEN;
    add_char_params.cccd_write_access = SEC_OPEN;

    err_code = characteristic_add(p_cb->service_handle,
                                  &add_char_params,
                                  &p_cb->tu_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add ADC characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid = CB_UUID_ADC_CHAR;
    add_char_params.uuid_type = p_cb->uuid_type;
    add_char_params.init_len = sizeof(uint8_t);
    add_char_params.max_len = sizeof(uint8_t);
    add_char_params.char_props.read = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access = SEC_OPEN;
    add_char_params.cccd_write_access = SEC_OPEN;

    return characteristic_add(p_cb->service_handle,
                              &add_char_params,
                              &p_cb->ADC_char_handles);
}

uint32_t ble_cb_tu_change(uint16_t conn_handle, ble_cb_t *p_cb, uint8_t button_state)
{
    ble_gatts_hvx_params_t params;
    uint16_t len = sizeof(button_state);    

    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_cb->tu_char_handles.value_handle;
    params.p_data = &button_state;
    params.p_len = &len;

    return sd_ble_gatts_hvx(conn_handle, &params);
}

uint32_t ble_cb_chuyendong_change(uint16_t conn_handle, ble_cb_t *p_cb, uint8_t button_state)
{
    ble_gatts_hvx_params_t params;
    uint16_t len = sizeof(button_state);

    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_cb->chuyendong_char_handles.value_handle;
    params.p_data = &button_state;
    params.p_len = &len;

    return sd_ble_gatts_hvx(conn_handle, &params);
}

uint32_t ble_cb_ADC_change(uint16_t conn_handle, ble_cb_t *p_cb, uint8_t adc_hex_val)
{
    ble_gatts_hvx_params_t params;
    uint16_t len = sizeof(adc_hex_val);

    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_cb->ADC_char_handles.value_handle;
    params.p_data = &adc_hex_val;
    params.p_len = &len;

    return sd_ble_gatts_hvx(conn_handle, &params);
}

void check_error_ble(ret_code_t err_code)
{
    if (err_code != NRF_SUCCESS &&
        err_code != BLE_ERROR_INVALID_CONN_HANDLE &&
        err_code != NRF_ERROR_INVALID_STATE &&
        err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
    {
        APP_ERROR_CHECK(err_code);
    }
};
/**
 * @}
 */
