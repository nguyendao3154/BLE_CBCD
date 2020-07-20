/**
 ******************************************************************************
 * @file    BLE_spec.h
 * @author  Makipos Co.,LTD.
 * @version 1.0
 * @date    May 28, 2020
 * @brief   
 * @history
 ******************************************************************************/
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
#include "app_sensor.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define DEVICE_NAME "MKP PIR Sensor" /**< Name of device. Will be included in the advertising data. */
#define MODEL_NUMBER "MKP_PIR_0001"
#define SERIAL_NUMBER "PIR_00000001"
#define MANUFACTURER_NAME "Makihome"
#define APP_BLE_OBSERVER_PRIO 3                                /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG 1                                 /**< A tag identifying the SoftDevice BLE configuration. */
#define APP_TIMER_PRESCALER 0                                  /**< Value of the RTC1 PRESCALER register. */
#define APP_ADV_INTERVAL 1600                                   /**< The advertising interval (in units of 0.625 ms; this value corresponds to 400 ms). */
#define APP_ADV_DURATION BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED /**< The advertising time-out (in units of seconds). When set to 0, we will never time out. */

#define MIN_CONN_INTERVAL MSEC_TO_UNITS(500, UNIT_1_25_MS) /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL MSEC_TO_UNITS(1000, UNIT_1_25_MS) /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY 0                                    /**< Slave latency. */
#define CONN_SUP_TIMEOUT MSEC_TO_UNITS(4000, UNIT_10_MS)   /**< Connection supervisory time-out (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(20000) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (15 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(5000)   /**< Time between each call to sd_ble_gap_conn_param_update after the first call (5 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT 3                        /**< Number of attempts before giving up the connection parameter negotiation. */

    void BLE_GapParamsInit(void);

    void BLE_GattInit(nrf_ble_gatt_t *p_gatt);

    void BLE_AdvertisingInit(ble_cb_t *p_cb);

    void BLE_ServicesInit(ble_cb_t *p_cb);

    void BLE_OnConnParamsEvt(ble_conn_params_evt_t *p_evt);

    void BLE_ConnParamsErrorHandler(uint32_t nrf_error);

    void BLE_ConnParamsInit(void);

    void BLE_AdvertisingStart(void);

    void BLE_EvtHandler(ble_evt_t const *p_ble_evt, void *p_context);

    void BLE_StackInit(void);

#ifdef __cplusplus
}
#endif

#endif

/**
 * @}
 */
