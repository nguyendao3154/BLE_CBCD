/**
 ******************************************************************************
 * @file    main.c
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
#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "BLE_spec.h"
#include "cambien_service.h"
#include "app_adc.h"
#include "app_sensor.h"
#include "app_led.h"
#include "app_ldr.h"
#include "user_timer.h"

#define DEAD_BEEF 0xDEADBEEF /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

uint8_t pir_sensitivity = 3;
uint8_t ldr_sensitivity = 1;
// bool m_saadc_initialized = false;
// volatile uint8_t u8pinvalue;

NRF_BLE_GATT_DEF(m_gatt); /**< GATT module instance. */
ble_cb_t m_cb;
NRF_SDH_BLE_OBSERVER(m_ble_observer,
                     BLE_CB_BLE_OBSERVER_PRIO,
                     ble_cb_on_ble_evt, &m_cb);

static void timers_init(void)
{
  // Initialize timer module, making it use the scheduler
  ret_code_t err_code = app_timer_init();
  APP_ERROR_CHECK(err_code);
  User_CreateTimer();
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

void get_mac_address(void)
{
		ble_gap_addr_t addr;
	ret_code_t err_code;
	err_code = sd_ble_gap_addr_get(&addr);
	APP_ERROR_CHECK(err_code);
	NRF_LOG_INFO("mac %x%x%x%x%x%x",addr.addr[0],addr.addr[1],addr.addr[2],addr.addr[3],addr.addr[4],addr.addr[5]);
}

int main(void)
{
  // Initialize.
  log_init();
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
  LED_Config();
  nrf_gpio_pin_clear(LED_PIN);
	
	get_mac_address();
  // nrf_drv_saadc_sample();
  //ADC_DeinitDriver();
  // Start execution.
  NRF_LOG_INFO("CBCD started.");
  // Enter main loop./s
  for (;;)
  {

    //SENSOR_MagneticTask();
    LDR_Process();
    SENSOR_PIR_Task();
    ADC_Task();
    LED_Task();
    NRF_LOG_FLUSH();
    nrf_pwr_mgmt_run();
    //sd_power_system_off();
  }
}

/**
 * @}
 */
