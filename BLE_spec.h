#ifndef BLE_SPEC_H__
#define BLE_SPEC_H__

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "boards.h"
#include "app_timer.h"
#include "app_button.h"
#include "nrf_ble_gatt.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_drv_saadc.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "cambien_service.h"
#include "app_adc.h"
#include "ble_dis_c.h"
#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_NAME "Door"                                 /**< Name of device. Will be included in the advertising data. */
#define MODEL_NUMBER "MKP_PIR_0001"
#define MANUFACTURER_NAME                "Makihome" 
#define APP_BLE_OBSERVER_PRIO 3                                /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG 1                                 /**< A tag identifying the SoftDevice BLE configuration. */
#define APP_TIMER_PRESCALER 0                                  /**< Value of the RTC1 PRESCALER register. */
#define APP_ADV_INTERVAL 640                                   /**< The advertising interval (in units of 0.625 ms; this value corresponds to 400 ms). */
#define APP_ADV_DURATION BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED /**< The advertising time-out (in units of seconds). When set to 0, we will never time out. */

#define MIN_CONN_INTERVAL MSEC_TO_UNITS(100, UNIT_1_25_MS)    /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL MSEC_TO_UNITS(200, UNIT_1_25_MS)    /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY 0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT MSEC_TO_UNITS(4000, UNIT_10_MS)      /**< Connection supervisory time-out (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(20000) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (15 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(5000)   /**< Time between each call to sd_ble_gap_conn_param_update after the first call (5 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT 3                        /**< Number of attempts before giving up the connection parameter negotiation. */

#define DEAD_BEEF 0xDEADBEEF /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */


extern uint16_t m_conn_handle;                                          /**< Handle of the current connection. */
extern uint8_t m_adv_handle;                                            /**< Advertising handle used to identify an advertising set. */
extern uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];            /**< Buffer for storing an encoded advertising set. */
extern uint8_t m_enc_scan_response_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX]; /**< Buffer for storing an encoded scan data. */
extern ble_gap_adv_data_t m_adv_data;
extern ble_cb_t m_cb;

void set_BLE_para(void);

void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name);

void gap_params_init(void);

void gatt_init(nrf_ble_gatt_t * p_gatt);

void advertising_init(ble_cb_t *p_cb);

void services_init(ble_cb_t *p_cb);

void on_conn_params_evt(ble_conn_params_evt_t * p_evt);

void conn_params_error_handler(uint32_t nrf_error);

void conn_params_init(void);

void advertising_start(void);

void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);

void ble_stack_init(void);

void log_init(void);

void power_management_init(void);

void idle_state_handle(void);

#ifdef __cplusplus
}
#endif

#endif
