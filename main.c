#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "BLE_spec.h"
#include "cambien_service.h"
#include "app_adc.h"
#include "app_sensor.h"

#define DEAD_BEEF 0xDEADBEEF /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

// bool m_saadc_initialized = false;
// volatile uint8_t u8pinvalue;

NRF_BLE_GATT_DEF(m_gatt); /**< GATT module instance. */
ble_cb_t m_cb;

static void timers_init(void)
{
    // Initialize timer module, making it use the scheduler
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
    ADC_CreateTimer();
    SENSOR_CreateTimer();
}

void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


int main(void)
{
    // Initialize.
    //log_init();
    timers_init();
    power_management_init();
    SENSOR_InterruptInit();
    SENSOR_MagneticGetInitialValue();
    ADC_Init();
    BLE_StackInit();
		sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
    BLE_GapParamsInit();
    BLE_GattInit(&m_gatt);
    BLE_ServicesInit(&m_cb);
    BLE_AdvertisingInit(&m_cb);
    BLE_ConnParamsInit();
    BLE_AdvertisingStart();
    nrf_drv_saadc_sample();
		//ADC_DeinitDriver();
    // Start execution.
    //NRF_LOG_INFO("CBCD started.");
    // Enter main loop.
    for (;;)
    {
	
        //SENSOR_MagneticTask();
        SENSOR_PIR_Task();
        ADC_Task();
        //NRF_LOG_FLUSH();
        nrf_pwr_mgmt_run();
    }
}

/**
 * @}
 */
