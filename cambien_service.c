
/**
 ******************************************************************************
 * @file    cambien_service.c
 * @author  Makipos Co.,LTD.
 * @version 1.0
 * @date    May 28, 2020
 * @brief   
 * @history
 * 
 *                      Revision History                                      *
 ****************************
 * Revision     Date            By              Description                         *
 ****************************
 * 1.0.0        28-May-2020     Nguyen Dao          create                                *

 ******************************************************************************/
/*******************************************************************************
 * Include
 ******************************************************************************/
#include "sdk_common.h"
#include "cambien_service.h"
#include "ble_srv_common.h"

extern uint8_t pir_sensitivity;
extern uint8_t ldr_sensitivity;
extern bool request_led_on;
extern uint16_t m_conn_handle;                                          /**< Handle of the current connection. */
extern uint8_t m_adv_handle;                                            /**< Advertising handle used to identify an advertising set. */
extern uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];            /**< Buffer for storing an encoded advertising set. */
extern uint8_t m_enc_scan_response_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX]; /**< Buffer for storing an encoded scan data. */
extern ble_cb_t m_cb;     

void ble_cb_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context)
{
    ble_cb_t *p_cb = (ble_cb_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GATTS_EVT_WRITE:
        on_write(p_cb, p_ble_evt);
        break;

    default:
        // No implementation needed.
        break;
    }
}

uint32_t BLECB_Init(ble_cb_t *p_cb, ble_cb_init_t *p_cb_init)
{
    uint32_t err_code;
    ble_uuid_t ble_uuid;
    ble_add_char_params_t add_char_params;

    p_cb->pir_write_handler = p_cb_init->pir_write_handler;

    // Add service.
    ble_uuid128_t base_uuid = {CB_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_cb->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_cb->uuid_type;
    ble_uuid.uuid = CB_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_cb->service_handle);
    VERIFY_SUCCESS(err_code);

    // Add magnetic characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid = CB_UUID_MAGNETIC_CHAR;
    add_char_params.uuid_type = p_cb->uuid_type;
    add_char_params.init_len = sizeof(uint8_t);
    add_char_params.max_len = sizeof(uint8_t);
    add_char_params.char_props.read = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access = SEC_OPEN;
    add_char_params.cccd_write_access = SEC_OPEN;

    err_code = characteristic_add(p_cb->service_handle,
                                  &add_char_params,
                                  &p_cb->magnetic_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    // Add chuyen dong characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid = CB_UUID_PIR_CHAR;
    add_char_params.uuid_type = p_cb->uuid_type;
    add_char_params.init_len = sizeof(uint8_t);
    add_char_params.max_len = sizeof(uint8_t);
    add_char_params.char_props.read = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access = SEC_OPEN;
    add_char_params.cccd_write_access = SEC_OPEN;

    err_code = characteristic_add(p_cb->service_handle,
                                  &add_char_params,
                                  &p_cb->pir_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add Write PIR characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid = CB_UUID_PIR_WRITE_CHAR;
    add_char_params.uuid_type = p_cb->uuid_type;
    add_char_params.init_len = sizeof(uint8_t);
    add_char_params.max_len = sizeof(uint8_t);
    add_char_params.char_props.read = 1;
    add_char_params.char_props.write = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access = SEC_OPEN;
    add_char_params.write_access = SEC_OPEN;
    add_char_params.cccd_write_access = SEC_OPEN;

    err_code = characteristic_add(p_cb->service_handle,
                                  &add_char_params,
                                  &p_cb->pir_write_char_handles);

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

    err_code = characteristic_add(p_cb->service_handle,
                                  &add_char_params,
                                  &p_cb->ADC_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    // Add Write LDR characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid = CB_UUID_LDR_WRITE_CHAR;
    add_char_params.uuid_type = p_cb->uuid_type;
    add_char_params.init_len = sizeof(uint8_t);
    add_char_params.max_len = sizeof(uint8_t);
    add_char_params.char_props.read = 1;
    add_char_params.char_props.write = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access = SEC_OPEN;
    add_char_params.write_access = SEC_OPEN;
    add_char_params.cccd_write_access = SEC_OPEN;

    return characteristic_add(p_cb->service_handle,
                                  &add_char_params,
                                  &p_cb->ldr_write_char_handles);

}

uint32_t BLECB_MagneticChange(uint16_t conn_handle, ble_cb_t *p_cb, uint8_t magnetic_state)
{
    ble_gatts_hvx_params_t params;
    uint16_t len = sizeof(magnetic_state);

    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_cb->magnetic_char_handles.value_handle;
    params.p_data = &magnetic_state;
    params.p_len = &len;

    return sd_ble_gatts_hvx(conn_handle, &params);
}

uint32_t BLECB_PIRChange(uint16_t conn_handle, ble_cb_t *p_cb, uint8_t PIR_state)
{
    ble_gatts_hvx_params_t params;
    uint16_t len = sizeof(PIR_state);

    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_cb->pir_char_handles.value_handle;
    params.p_data = &PIR_state;
    params.p_len = &len;

    return sd_ble_gatts_hvx(conn_handle, &params);
}

uint32_t BLECB_PIRWriteChange(uint16_t conn_handle, ble_cb_t *p_cb, uint8_t PIR_state)
{
    ble_gatts_hvx_params_t params;
    uint16_t len = sizeof(PIR_state);

    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_cb->pir_write_char_handles.value_handle;
    params.p_data = &PIR_state;
    params.p_len = &len;

    return sd_ble_gatts_hvx(conn_handle, &params);
}

uint32_t BLECB_ADCChange(uint16_t conn_handle, ble_cb_t *p_cb, uint8_t adc_hex_val)
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

uint32_t BLECB_LDRChange(uint16_t conn_handle, ble_cb_t *p_cb, uint8_t ldr_hex_val)
{
    ble_gatts_hvx_params_t params;
    uint16_t len = sizeof(ldr_hex_val);

    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_cb->ldr_char_handles.value_handle;
    params.p_data = &ldr_hex_val;
    params.p_len = &len;

    return sd_ble_gatts_hvx(conn_handle, &params);
}

uint32_t BLECB_LDRWriteChange(uint16_t conn_handle, ble_cb_t *p_cb, uint8_t ldr_hex_val)
{
    ble_gatts_hvx_params_t params;
    uint16_t len = sizeof(ldr_hex_val);

    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_cb->ldr_write_char_handles.value_handle;
    params.p_data = &ldr_hex_val;
    params.p_len = &len;

    return sd_ble_gatts_hvx(conn_handle, &params);
}

void BLECB_CheckError(ret_code_t err_code)
{
    if (err_code != NRF_SUCCESS &&
        err_code != BLE_ERROR_INVALID_CONN_HANDLE &&
        err_code != NRF_ERROR_INVALID_STATE &&
        err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
    {
        APP_ERROR_CHECK(err_code);
    }
};

void on_write(ble_cb_t *p_cb, ble_evt_t const *p_ble_evt)
{
    ble_gatts_evt_write_t const *p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if ((p_evt_write->handle == p_cb->pir_write_char_handles.value_handle) &&
        (p_evt_write->len == 1))
    //(p_cb->pir_write_handler != NULL)
    {
        request_led_on = true;
        // p_cb->pir_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_cb, p_evt_write->data[0]);
        if ((p_evt_write->data[0] > PIR_MIN_SENSITIVITY - 1) && (p_evt_write->data[0] < PIR_MAX_SENSITIVITY + 1))
        {
            pir_sensitivity = p_evt_write->data[0];
        }
        else
        {
            BLECB_PIRWriteChange(m_conn_handle, &m_cb, pir_sensitivity);
        }
    }
    if ((p_evt_write->handle == p_cb->ldr_write_char_handles.value_handle) &&
        (p_evt_write->len == 1))
    //(p_cb->pir_write_handler != NULL)
    {
        request_led_on = true;
        // p_cb->pir_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_cb, p_evt_write->data[0]);
        if ((p_evt_write->data[0] > LDR_MIN_SENSITIVITY - 1) && (p_evt_write->data[0] < LDR_MAX_SENSITIVITY + 1))
        {
            ldr_sensitivity = p_evt_write->data[0];
        }
        else
        {
            BLECB_LDRWriteChange(m_conn_handle, &m_cb, ldr_sensitivity);
        }
    }
}


/**
 * @}
 */
