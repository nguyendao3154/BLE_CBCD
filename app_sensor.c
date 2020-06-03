/**
 ******************************************************************************
 * @file    app_interrupt.c
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
#include "app_sensor.h"

extern uint16_t m_conn_handle;                                          /**< Handle of the current connection. */
extern uint8_t m_adv_handle;                                            /**< Advertising handle used to identify an advertising set. */
extern uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];            /**< Buffer for storing an encoded advertising set. */
extern uint8_t m_enc_scan_response_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX]; /**< Buffer for storing an encoded scan data. */
extern ble_cb_t m_cb;

uint8_t pir_state;
uint8_t pir_pre_state = 0, task_state = 0, task_pre_state = 0;
uint8_t magnetic_logic_level;
bool MAGNETIC_FLAG = false;
uint8_t numof1000ticks;
APP_TIMER_DEF(timer_systick_id);

void SENSOR_MagneticHandle(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    MAGNETIC_FLAG = true;
    if (!nrf_gpio_pin_read(MAGNETIC_PIN))
    {
        magnetic_logic_level = 1;
        NRF_LOG_INFO("Co nam cham\r\n");
    }
    else
    {
        magnetic_logic_level = 0;
        //NRF_LOG_INFO("Xa nam cham\r\n");
    }
}

void SENSOR_MagneticInit(void)
{
    nrf_drv_gpiote_in_config_t magnetic_config;
    magnetic_config.pull = NRF_GPIO_PIN_PULLUP;
    magnetic_config.sense = NRF_GPIOTE_POLARITY_TOGGLE;
    magnetic_config.is_watcher = false;
    magnetic_config.hi_accuracy = false;
    nrf_drv_gpiote_in_init(MAGNETIC_PIN,
                           &magnetic_config,
                           SENSOR_MagneticHandle);
    nrf_drv_gpiote_in_event_enable(MAGNETIC_PIN, true);
}
void SENSOR_PIR_Out1Handle(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (nrf_gpio_pin_read(PIR_OUT1_PIN))
    {
        pir_state = 1;
        NRF_LOG_INFO("CD 1\r\n");
    }
    else
    {
        pir_state = 0;
        NRF_LOG_INFO("CD 0\r\n");
    }
}

void SENSOR_PIR_OUT1Init(void)
{
    nrf_drv_gpiote_in_config_t out1_config;
    //out1_config.pull = NRF_GPIO_PIN_PULLUP;
    out1_config.pull = NRF_GPIO_PIN_PULLDOWN;
    out1_config.sense = NRF_GPIOTE_POLARITY_TOGGLE;
    out1_config.is_watcher = false;
    out1_config.hi_accuracy = false;
    nrf_drv_gpiote_in_init(PIR_OUT1_PIN,
                           &out1_config,
                           SENSOR_PIR_Out1Handle);
    nrf_drv_gpiote_in_event_enable(PIR_OUT1_PIN, true);
}
void SENSOR_PIR_Out2Handle(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (nrf_gpio_pin_read(PIR_OUT2_PIN))
    {
        pir_state = 1;
        NRF_LOG_INFO("CD 1\r\n");
    }
    else
    {
        pir_state = 0;
        NRF_LOG_INFO("CD 0\r\n");
    }
}

void SENSOR_PIR_OUT2Init(void)
{
    nrf_drv_gpiote_in_config_t out2_config;
    //out2_config.pull = NRF_GPIO_PIN_PULLUP;
    out2_config.pull = NRF_GPIO_PIN_PULLDOWN;
    out2_config.sense = NRF_GPIOTE_POLARITY_TOGGLE;
    out2_config.is_watcher = false;
    out2_config.hi_accuracy = false;
    nrf_drv_gpiote_in_init(PIR_OUT2_PIN,
                           &out2_config,
                           SENSOR_PIR_Out2Handle);
    nrf_drv_gpiote_in_event_enable(PIR_OUT2_PIN, true);
}
void SENSOR_InterruptInit(void)
{
    nrf_drv_gpiote_init();
    SENSOR_MagneticInit();
    SENSOR_PIR_OUT1Init();
    SENSOR_PIR_OUT2Init();
}
void SENSOR_MagneticTask(void)
{
    ret_code_t err_code;
    if (MAGNETIC_FLAG)
    {
        err_code = BLECB_MagneticChange(m_conn_handle, &m_cb, magnetic_logic_level);

        BLECB_CheckError(err_code);
        MAGNETIC_FLAG = false;
    }
}
void SENSOR_PIR_Task(void)
{
    ret_code_t err_code;

    if ((numof1000ticks > PIR_TIMEOUT) && !pir_state) // ko co gi
    {
        task_state = 2;
        NRF_LOG_INFO("task 2");
    }
    if ((numof1000ticks > PIR_TIMEOUT) && pir_state) // co chuyen dong
    {
        task_state = 1;
        NRF_LOG_INFO("task 1");
    }
    if (task_pre_state != task_state)
    {
        err_code = BLECB_PIRChange(m_conn_handle, &m_cb, pir_state);
        BLECB_CheckError(err_code);
    }
    if (pir_state)
    {
        numof1000ticks = 0;
    }
    //NRF_LOG_INFO("%d\n", numof1000ticks);
    // NRF_LOG_INFO("%d %d %d %d\n", pir_pre_state, pir_state, task_pre_state, task_state);
    pir_pre_state = pir_state;
    task_pre_state = task_state;
}

void SENSOR_TickCount(void *p_context)
{
    numof1000ticks++;
}

void SENSOR_CreateTimer(void)
{
    ret_code_t err_code;
    err_code = app_timer_create(&timer_systick_id,
                                APP_TIMER_MODE_REPEATED,
                                SENSOR_TickCount);
    APP_ERROR_CHECK(err_code);
    APP_ERROR_CHECK(app_timer_start(timer_systick_id, APP_TIMER_TICKS(1000), NULL));
}

/**
 * @}
 */
