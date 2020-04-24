/**
 * Copyright (c) 2015 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/**
 * @brief Blinky Sample Application main file.
 *
 * This file contains the source code for a sample server application using the LED Button service.
 */

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
#include "nrf_drv_gpiote.h"
#include "nrf_drv_saadc.h"
#define TU_PIN 3
#define OUT1_PIN 18
#define OUT2_PIN 13

#define DEVICE_NAME "Sieu Pham"                                 /**< Name of device. Will be included in the advertising data. */
#define APP_BLE_OBSERVER_PRIO 3                                /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG 1                                 /**< A tag identifying the SoftDevice BLE configuration. */
#define APP_TIMER_PRESCALER 0                                  /**< Value of the RTC1 PRESCALER register. */
#define APP_ADV_INTERVAL 640                                   /**< The advertising interval (in units of 0.625 ms; this value corresponds to 40 ms). */
#define APP_ADV_DURATION BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED /**< The advertising time-out (in units of seconds). When set to 0, we will never time out. */

#define MIN_CONN_INTERVAL MSEC_TO_UNITS(100, UNIT_1_25_MS)    /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL MSEC_TO_UNITS(200, UNIT_1_25_MS)    /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY 0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT MSEC_TO_UNITS(4000, UNIT_10_MS)      /**< Connection supervisory time-out (4 seconds). */
#define PIR_TIMEOUT 5                                        // PIR timeout 10s
#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(20000) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (15 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(5000)   /**< Time between each call to sd_ble_gap_conn_param_update after the first call (5 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT 3                        /**< Number of attempts before giving up the connection parameter negotiation. */

#define DEAD_BEEF 0xDEADBEEF /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define ADC_TIME_SCAN 86400000 // ADC quet 1 ngay 1 lan
#define AIN_PIR_CHANNEL NRF_SAADC_INPUT_AIN0
#define AIN_BAT_CHANNEL NRF_SAADC_INPUT_AIN1

#define SAMPLES_IN_BUFFER 1

static nrf_saadc_value_t m_buffer[SAMPLES_IN_BUFFER];

APP_TIMER_DEF(m_adc_id);
APP_TIMER_DEF(timer_systick_id);
NRF_BLE_GATT_DEF(m_gatt); /**< GATT module instance. */
BLE_CB_DEF(m_cb);

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; /**< Handle of the current connection. */

static uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;           /**< Advertising handle used to identify an advertising set. */
static uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];            /**< Buffer for storing an encoded advertising set. */
static uint8_t m_enc_scan_response_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX]; /**< Buffer for storing an encoded scan data. */
static uint8_t pin_8bit_value;
static uint32_t pin_value;
bool adc_flag = false;
static bool m_saadc_initialized = false;
/**@brief Struct that contains pointers to the encoded advertising data. */
uint32_t sys_tick;
uint8_t pir_state;
uint16_t pir_analog_value;
static ble_gap_adv_data_t m_adv_data =
    {
        .adv_data =
            {
                .p_data = m_enc_advdata,
                .len = BLE_GAP_ADV_SET_DATA_SIZE_MAX},
        .scan_rsp_data =
            {
                .p_data = m_enc_scan_response_data,
                .len = BLE_GAP_ADV_SET_DATA_SIZE_MAX

            }};

void saadc_callback(nrf_drv_saadc_evt_t const *p_event)
{
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
        adc_flag = true;

        ret_code_t err_code;
        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLES_IN_BUFFER);
        APP_ERROR_CHECK(err_code);
        // pin_value = (p_event->data.done.p_buffer[0]) * 12 * 11 / 1024;
        // pin_8bit_value = (uint8_t)pin_value;
        pir_analog_value = (p_event->data.done.p_buffer[0]);
        NRF_LOG_INFO("%d\r\n", pir_analog_value);
        nrf_drv_saadc_uninit();                                                     //Unintialize SAADC to disable EasyDMA and save power
        NRF_SAADC->INTENCLR = (SAADC_INTENCLR_END_Clear << SAADC_INTENCLR_END_Pos); //Disable the SAADC interrupt
        NVIC_ClearPendingIRQ(SAADC_IRQn);                                           //Clear the SAADC interrupt if set
        m_saadc_initialized = false;
    }
}
void saadc_init(void)
{
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel1_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(AIN_PIR_CHANNEL);

    // nrf_saadc_channel_config_t channel2_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(AIN_BAT_CHANNEL);

    err_code = nrf_drv_saadc_init(NULL, saadc_callback);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_channel_init(0, &channel1_config);
    APP_ERROR_CHECK(err_code);

    // err_code = nrf_drv_saadc_channel_init(1, &channel2_config);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_buffer_convert(m_buffer, SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);
}

static void adc_handle_timer(void *p_context)
{

    if (!m_saadc_initialized)
    {   
        saadc_init(); //Initialize the SAADC. In the case when SAADC_SAMPLES_IN_BUFFER > 1 then we only need to initialize the SAADC when the the buffer is empty.
    }
    m_saadc_initialized = true;
}

static void create_ADC_timer()
{
    ret_code_t err_code;
    // Create timers
    err_code = app_timer_create(&m_adc_id,
                                APP_TIMER_MODE_REPEATED,
                                adc_handle_timer);
    APP_ERROR_CHECK(err_code);
    APP_ERROR_CHECK(app_timer_start(m_adc_id, APP_TIMER_TICKS(ADC_TIME_SCAN), NULL));
}

void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

bool TU_FLAG = false;
uint8_t tu_logic_level;
void in_tu_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    TU_FLAG = true;
    if (!nrf_gpio_pin_read(TU_PIN))
    {
        tu_logic_level = 1;
        NRF_LOG_INFO("Co nam cham\r\n");
    }
    else
    {
        tu_logic_level = 0;
        NRF_LOG_INFO("Xa nam cham\r\n");
    }
}

void tu_interrupt_init()
{
    nrf_drv_gpiote_in_config_t tu_config;
    tu_config.pull = NRF_GPIO_PIN_PULLUP;
    tu_config.sense = NRF_GPIOTE_POLARITY_TOGGLE;
    tu_config.is_watcher = false;
    tu_config.hi_accuracy = false;
    nrf_drv_gpiote_in_init(TU_PIN,
                           &tu_config,
                           in_tu_handler);
    nrf_drv_gpiote_in_event_enable(TU_PIN, true);
}
void in_out1_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (nrf_gpio_pin_read(OUT1_PIN))
    {
        pir_state = 1;
        NRF_LOG_INFO("CD 1\r\n");
    }
    else
    {
        pir_state = 0;
        //NRF_LOG_INFO("CD 1\r\n");
    }
}

void out1_interrupt_init()
{
    nrf_drv_gpiote_in_config_t out1_config;
    //out1_config.pull = NRF_GPIO_PIN_PULLUP;
    out1_config.pull = NRF_GPIO_PIN_PULLDOWN;
    out1_config.sense = NRF_GPIOTE_POLARITY_TOGGLE;
    out1_config.is_watcher = false;
    out1_config.hi_accuracy = false;
    nrf_drv_gpiote_in_init(OUT1_PIN,
                           &out1_config,
                           in_out1_handler);
    nrf_drv_gpiote_in_event_enable(OUT1_PIN, true);
}
void in_out2_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (nrf_gpio_pin_read(OUT2_PIN))
    {
        pir_state = 1;
        NRF_LOG_INFO("CD 2\r\n");
    }
    else
    {
        pir_state = 0;
        //NRF_LOG_INFO("CD 2\r\n");
    }
}

void out2_interrupt_init()
{
    nrf_drv_gpiote_in_config_t out2_config;
    // out2_config.pull = NRF_GPIO_PIN_PULLUP;
    out2_config.pull = NRF_GPIO_PIN_PULLDOWN;
    out2_config.sense = NRF_GPIOTE_POLARITY_TOGGLE;
    out2_config.is_watcher = false;
    out2_config.hi_accuracy = false;
    nrf_drv_gpiote_in_init(OUT2_PIN,
                           &out2_config,
                           in_out2_handler);
    nrf_drv_gpiote_in_event_enable(OUT2_PIN, true);
}
void interrupt_init()
{
    nrf_drv_gpiote_init();
    //tu_interrupt_init();
    out1_interrupt_init();
    out2_interrupt_init();
}
void systick_handle(void *p_context)
{
    sys_tick++;
}

static void timers_init(void)
{
    // Initialize timer module, making it use the scheduler
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&timer_systick_id,
                                APP_TIMER_MODE_REPEATED,
                                systick_handle);
    APP_ERROR_CHECK(err_code);
    APP_ERROR_CHECK(app_timer_start(timer_systick_id, APP_TIMER_TICKS(1000), NULL));
}

static void gap_params_init(void)
{
    ret_code_t err_code;
    ble_gap_conn_params_t gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}

static void advertising_init(void)
{
    ret_code_t err_code;
    ble_advdata_t advdata;
    ble_advdata_t srdata;

    ble_uuid_t adv_uuids[] = {{CB_UUID_SERVICE, m_cb.uuid_type}};

    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance = true;
    advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    memset(&srdata, 0, sizeof(srdata));
    srdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    srdata.uuids_complete.p_uuids = adv_uuids;

    err_code = ble_advdata_encode(&advdata, m_adv_data.adv_data.p_data, &m_adv_data.adv_data.len);
    APP_ERROR_CHECK(err_code);

    err_code = ble_advdata_encode(&srdata, m_adv_data.scan_rsp_data.p_data, &m_adv_data.scan_rsp_data.len);
    APP_ERROR_CHECK(err_code);

    ble_gap_adv_params_t adv_params;

    // Set advertising parameters.
    memset(&adv_params, 0, sizeof(adv_params));

    adv_params.primary_phy = BLE_GAP_PHY_1MBPS;
    adv_params.duration = APP_ADV_DURATION;
    adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
    adv_params.p_peer_addr = NULL;
    adv_params.filter_policy = BLE_GAP_ADV_FP_ANY;
    adv_params.interval = APP_ADV_INTERVAL;

    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, &adv_params);
    APP_ERROR_CHECK(err_code);
}

static void services_init(void)
{
    uint32_t err_code;

    err_code = ble_cb_init(&m_cb);
    APP_ERROR_CHECK(err_code);
}

static void on_conn_params_evt(ble_conn_params_evt_t *p_evt)
{
    ret_code_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

static void conn_params_init(void)
{
    ret_code_t err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail = false;
    cp_init.evt_handler = on_conn_params_evt;
    cp_init.error_handler = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

static void advertising_start(void)
{
    ret_code_t err_code;

    err_code = sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
}

static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context)
{
    ret_code_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        NRF_LOG_INFO("Connected");
        m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
        err_code = ble_cb_ADC_change(m_conn_handle, &m_cb, pir_analog_value);

        check_error_ble(err_code);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        NRF_LOG_INFO("Disconnected");
        m_conn_handle = BLE_CONN_HANDLE_INVALID;
        advertising_start();
        break;

    case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
        // Pairing not supported
        err_code = sd_ble_gap_sec_params_reply(m_conn_handle,
                                               BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP,
                                               NULL,
                                               NULL);
        APP_ERROR_CHECK(err_code);
        break;

    case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
    {
        NRF_LOG_DEBUG("PHY update request.");
        ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
        err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
        APP_ERROR_CHECK(err_code);
    }
    break;

    case BLE_GATTS_EVT_SYS_ATTR_MISSING:
        // No system attributes have been stored.
        err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
        APP_ERROR_CHECK(err_code);
        break;

    case BLE_GATTC_EVT_TIMEOUT:
        // Disconnect on GATT Client timeout event.
        NRF_LOG_DEBUG("GATT Client Timeout.");
        err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                         BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
        break;

    case BLE_GATTS_EVT_TIMEOUT:
        // Disconnect on GATT Server timeout event.
        NRF_LOG_DEBUG("GATT Server Timeout.");
        err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                         BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
        break;

    default:
        // No implementation needed.
        break;
    }
}

static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}
void task_tu()
{
    ret_code_t err_code;
    if (TU_FLAG)
    {

        err_code = ble_cb_tu_change(m_conn_handle, &m_cb, tu_logic_level);

        check_error_ble(err_code);
        TU_FLAG = false;
    }
}
uint8_t pir_pre_state = 0, task_state = 0, task_pre_state = 0;

void task_chuyendong()
{
    ret_code_t err_code;

    if ((sys_tick > PIR_TIMEOUT) && !pir_state) // ko co gi
    {
        task_state = 2;
        // NRF_LOG_INFO("task 2");
    }
    if ((sys_tick > PIR_TIMEOUT) && pir_state) // co chuyen dong
    {
        task_state = 1;
        // NRF_LOG_INFO("task 1");
    }
    if (task_pre_state != task_state)
    {
        err_code = ble_cb_chuyendong_change(m_conn_handle, &m_cb, pir_state);
        check_error_ble(err_code);
    }
    if (pir_state)
    {
        sys_tick = 0;
    }
    // NRF_LOG_INFO("%d\n", sys_tick);
    // NRF_LOG_INFO("%d %d %d %d\n", pir_pre_state, pir_state, task_pre_state, task_state);
    pir_pre_state = pir_state;
    task_pre_state = task_state;
}
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

static void idle_state_handle(void)
{

    nrf_pwr_mgmt_run();
}
void task_adc(void)
{
    ret_code_t err_code;
    nrf_drv_saadc_sample();
    if (adc_flag)
    {
        err_code = ble_cb_ADC_change(m_conn_handle, &m_cb, pir_analog_value);

        check_error_ble(err_code);
        adc_flag = false;
    }
}
int main(void)
{
    // Initialize.
    //log_init();
    timers_init();
    create_ADC_timer();
    power_management_init();
    interrupt_init();
    saadc_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    advertising_init();
    conn_params_init();
    nrf_drv_saadc_sample();
    // Start execution.
    //check_error_ble(ble_cb_ADC_change(m_conn_handle, &m_cb, pir_analog_value));
    
    NRF_LOG_INFO("Sieu pham started.");
    advertising_start();

    // Enter main loop.
    for (;;)
    {

        //task_tu();
        task_chuyendong();
        task_adc();
        //NRF_LOG_FLUSH();
        
        idle_state_handle();

        
    }
}

/**
 * @}
 */
