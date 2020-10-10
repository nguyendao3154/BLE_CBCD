
/**
 ******************************************************************************
 * @file    cambien_service.h
 * @author  Makipos Co.,LTD.
 * @version 1.0
 * @date    May 28, 2020
 * @brief   
 * @history
 ******************************************************************************/
#ifndef CAMBIEN_SERVICE_H__
#define CAMBIEN_SERVICE_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "app_error.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define BLE_CB_BLE_OBSERVER_PRIO 2
#define BLE_CB_DEF(_name)                          \
    static ble_cb_t _name;                         \
    NRF_SDH_BLE_OBSERVER(_name##_obs,              \
                         BLE_CB_BLE_OBSERVER_PRIO, \
                         ble_cb_on_ble_evt, &_name)

#define CB_UUID_BASE                                       \
    {                                                      \
        0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,    \
            0x00, 0x10, 0x00, 0x00, 0x34, 0x12, 0x00, 0x00 \
    }
#define CB_UUID_SERVICE 0x1234
#define CB_UUID_MAGNETIC_CHAR 0x1235
#define CB_UUID_PIR_CHAR 0x1236
#define CB_UUID_PIR_WRITE_CHAR 0x1237
#define CB_UUID_ADC_CHAR 0x1238
#define CB_UUID_LDR_WRITE_CHAR 0x1239
#define CB_UUID_LDR_READ_CHAR 0x1240

    typedef struct ble_cb_s ble_cb_t;

typedef void (*ble_cb_pir_write_handler_t) (uint16_t conn_handle, ble_cb_t * p_cb, uint8_t new_state);
typedef void (*ble_cb_ldr_write_handler_t) (uint16_t conn_handle, ble_cb_t * p_cb, uint8_t new_state);

typedef struct
{
    ble_cb_pir_write_handler_t pir_write_handler; /**< Event handler to be called when the PIR Characteristic is written. */
    ble_cb_ldr_write_handler_t ldr_write_handler; /**< Event handler to be called when the LDR Characteristic is written. */

} ble_cb_init_t;

    struct ble_cb_s
    {
        uint16_t service_handle;
        ble_gatts_char_handles_t magnetic_char_handles;
        ble_gatts_char_handles_t pir_char_handles;
        ble_gatts_char_handles_t ADC_char_handles;
        ble_gatts_char_handles_t ldr_char_handles;
        ble_gatts_char_handles_t pir_write_char_handles;
        ble_gatts_char_handles_t ldr_write_char_handles;
        ble_cb_pir_write_handler_t pir_write_handler;
        ble_cb_ldr_write_handler_t ldr_write_handler;
        uint8_t uuid_type;
    };

typedef enum{
    PIR_MAX_SENSITIVITY = 8,
    PIR_MIN_SENSITIVITY = 1,
    LDR_MAX_SENSITIVITY = 5,
    LDR_MIN_SENSITIVITY = 1
}write_sensitivity;

    uint32_t BLECB_Init(ble_cb_t *p_cb, ble_cb_init_t *p_cb_init);

    uint32_t BLECB_MagneticChange(uint16_t conn_handle, ble_cb_t *p_cb, uint8_t magnetic_state);

    uint32_t BLECB_PIRChange(uint16_t conn_handle, ble_cb_t *p_cb, uint8_t PIR_state);

    uint32_t BLECB_ADCChange(uint16_t conn_handle, ble_cb_t *p_cb, uint8_t adc_hex_val);

    uint32_t BLECB_LDRChange(uint16_t conn_handle, ble_cb_t *p_cb, uint8_t ldr_hex_val);

    uint32_t BLECB_PIRWriteChange(uint16_t conn_handle, ble_cb_t *p_cb, uint8_t PIR_state);

    uint32_t BLECB_LDRWriteChange(uint16_t conn_handle, ble_cb_t *p_cb, uint8_t ldr_hex_val);

    void BLECB_CheckError(ret_code_t err_code);

    void ble_cb_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

    void on_write(ble_cb_t *p_cb, ble_evt_t const *p_ble_evt);
#ifdef __cplusplus
}
#endif

#endif

/**
 * @}
 */
