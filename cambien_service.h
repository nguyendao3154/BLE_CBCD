

#ifndef CAMBIEN_SERVICE_H__
#define CAMBIEN_SERVICE_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "app_error.h"

#ifdef __cplusplus
extern "C" {
#endif
#define BLE_CB_BLE_OBSERVER_PRIO 2
#define BLE_CB_DEF(_name)                                                                          \
static ble_cb_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_CB_BLE_OBSERVER_PRIO,                                                     \
                     ble_cb_on_ble_evt, &_name)

#define CB_UUID_BASE        {0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, \
                              0x00, 0x10, 0x00, 0x00, 0x34, 0x12, 0x00, 0x00}                   
#define CB_UUID_SERVICE             0x1234
#define Cb_UUID_TU_CHAR             0x1235
#define CB_UUID_CHUYENDONG_CHAR     0x1236
#define CB_UUID_ADC_CHAR            0x1238

typedef struct ble_cb_s ble_cb_t;

struct ble_cb_s
{
    uint16_t                    service_handle;      
    ble_gatts_char_handles_t    tu_char_handles;   
    ble_gatts_char_handles_t    chuyendong_char_handles; 
    ble_gatts_char_handles_t    ADC_char_handles;
    uint8_t                     uuid_type;           
};

uint32_t ble_cb_init(ble_cb_t * p_cb);

void ble_cb_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_contex);

uint32_t ble_cb_tu_change(uint16_t conn_handle, ble_cb_t * p_cb, uint8_t button_state);

uint32_t ble_cb_chuyendong_change(uint16_t conn_handle, ble_cb_t * p_cb, uint8_t button_state);

uint32_t ble_cb_ADC_change(uint16_t conn_handle, ble_cb_t * p_cb, uint8_t adc_hex_val);

void check_error_ble(ret_code_t err_code);
#ifdef __cplusplus
}
#endif

#endif 
