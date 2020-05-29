#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "BLE_spec.h"
#include "cambien_service.h"
#include "app_adc.h"
#include "app_interrupt.h"


bool m_saadc_initialized = false;
volatile uint8_t pin_8bit_value;

NRF_BLE_GATT_DEF(m_gatt); /**< GATT module instance. */
ble_cb_t m_cb;

uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; /**< Handle of the current connection. */

uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;           /**< Advertising handle used to identify an advertising set. */
uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];            /**< Buffer for storing an encoded advertising set. */
uint8_t m_enc_scan_response_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX]; /**< Buffer for storing an encoded scan data. */
bool adc_ready = false;

/**@brief Struct that contains pointers to the encoded advertising data. */

ble_gap_adv_data_t m_adv_data;

static void timers_init(void)
{
    // Initialize timer module, making it use the scheduler
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
    create_ADC_timer();
    create_sensor_timer();

}

static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

int main(void)
{
    // Initialize.
    set_BLE_para();
    log_init();
    timers_init();
    power_management_init();
    interrupt_init();
    saadc_init();
    ble_stack_init();
    gap_params_init();
    gatt_init(&m_gatt);
    services_init(&m_cb);
    advertising_init(&m_cb);
    conn_params_init();
    nrf_drv_saadc_sample();
    // Start execution.
    //check_error_ble(ble_cb_ADC_change(m_conn_handle, &m_cb, get_adc_value()));

    NRF_LOG_INFO("CBCD started.");
    advertising_start();

    // Enter main loop.
    for (;;)
    {

        task_tu();
        task_chuyendong(); 
        task_adc();
        NRF_LOG_FLUSH();

        nrf_pwr_mgmt_run();
    }
}

/**
 * @}
 */
