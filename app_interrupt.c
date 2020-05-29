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
#include "app_interrupt.h"

uint8_t pir_state;
uint8_t pir_pre_state = 0, task_state = 0, task_pre_state = 0;
uint8_t tu_logic_level;
bool TU_FLAG = false;
uint32_t sys_tick;
APP_TIMER_DEF(timer_systick_id);

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
        //NRF_LOG_INFO("Xa nam cham\r\n");
    }
}

void tu_interrupt_init(void)
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
        NRF_LOG_INFO("CD 0\r\n");
    }
}

void out1_interrupt_init(void)
{
    nrf_drv_gpiote_in_config_t out1_config;
    out1_config.pull = NRF_GPIO_PIN_PULLUP;
    //out1_config.pull = NRF_GPIO_PIN_PULLDOWN;
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
        NRF_LOG_INFO("CD 1\r\n");
    }
    else
    {
        pir_state = 0;
        NRF_LOG_INFO("CD 0\r\n");
    }
}

void out2_interrupt_init(void)
{
    nrf_drv_gpiote_in_config_t out2_config;
    out2_config.pull = NRF_GPIO_PIN_PULLUP;
    //out2_config.pull = NRF_GPIO_PIN_PULLDOWN;
    out2_config.sense = NRF_GPIOTE_POLARITY_TOGGLE;
    out2_config.is_watcher = false;
    out2_config.hi_accuracy = false;
    nrf_drv_gpiote_in_init(OUT2_PIN,
                           &out2_config,
                           in_out2_handler);
    nrf_drv_gpiote_in_event_enable(OUT2_PIN, true);
}
void interrupt_init(void)
{
    nrf_drv_gpiote_init();
    tu_interrupt_init();
    out1_interrupt_init();
    out2_interrupt_init();
}
void task_tu(void)
{
    ret_code_t err_code;
    if (TU_FLAG)
    {
        err_code = ble_cb_tu_change(m_conn_handle, &m_cb, tu_logic_level);

        check_error_ble(err_code);
        TU_FLAG = false;
    }
}
void task_chuyendong(void)
{
    ret_code_t err_code;

    if ((sys_tick > PIR_TIMEOUT) && !pir_state) // ko co gi
    {
        task_state = 2;
         NRF_LOG_INFO("task 2");
    }
    if ((sys_tick > PIR_TIMEOUT) && pir_state) // co chuyen dong
    {
        task_state = 1;
         NRF_LOG_INFO("task 1");
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
     //NRF_LOG_INFO("%d\n", sys_tick);
    // NRF_LOG_INFO("%d %d %d %d\n", pir_pre_state, pir_state, task_pre_state, task_state);
    pir_pre_state = pir_state;
    task_pre_state = task_state;
}

void systick_handle(void *p_context)
{
    sys_tick++;
}

void create_sensor_timer(void)
{
    ret_code_t err_code;
    err_code = app_timer_create(&timer_systick_id,
                                APP_TIMER_MODE_REPEATED,
                                systick_handle);
    APP_ERROR_CHECK(err_code);
    APP_ERROR_CHECK(app_timer_start(timer_systick_id, APP_TIMER_TICKS(1000), NULL));
}

