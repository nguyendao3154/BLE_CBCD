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


#define PIR_TIMEOUT 100             // 10s
#define DOOR_MINIMUM_INTERVAL 5     // 0.5s
#define DOOR_MAXIMUM_INTERVAL 8
#define DEFAULT_MAGNETIC_STATE 3
extern uint16_t m_conn_handle;                                          /**< Handle of the current connection. */
extern uint8_t m_adv_handle;                                            /**< Advertising handle used to identify an advertising set. */
extern uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];            /**< Buffer for storing an encoded advertising set. */
extern uint8_t m_enc_scan_response_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX]; /**< Buffer for storing an encoded scan data. */
extern ble_cb_t m_cb;

bool magnetic_flag = false;
uint8_t volatile pir_task_state = 1;
uint8_t pir_logic_level, pir_task_pre_state;
uint8_t volatile magnetic_logic_level;
uint8_t magnetic_task_pre_state = DEFAULT_MAGNETIC_STATE;
uint32_t numsof100ticks_pir;
uint32_t numsof100ticks_door;
APP_TIMER_DEF(timer_systick_id);

void SENSOR_MagneticGetInitialValue(void)
{
    magnetic_logic_level = !nrf_gpio_pin_read(MAGNETIC_PIN);
}
void SENSOR_MagneticHandle(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    magnetic_flag = true;
    if (!nrf_gpio_pin_read(MAGNETIC_PIN))
    {
        magnetic_logic_level = 1;
        //NRF_LOG_INFO("Co nam cham\r\n");
    }
    else
    {
        magnetic_logic_level = 0;
        // NRF_LOG_INFO("Xa nam cham\r\n");
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
        pir_logic_level = 1;
        //NRF_LOG_INFO("CD 1\r\n");
    }
    else
    {
        pir_logic_level = 0;
        //NRF_LOG_INFO("CD 0\r\n");
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
        pir_logic_level = 1;
        //NRF_LOG_INFO("CD 1\r\n");
    }
    else
    {
        pir_logic_level = 0;
        //NRF_LOG_INFO("CD 0\r\n");
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
    if (magnetic_flag)
    {
        /* thay doi trang thai trong khoang thoi gian lon hon 0.5s */
        if ((numsof100ticks_door > DOOR_MINIMUM_INTERVAL) && (magnetic_task_pre_state != magnetic_logic_level))
        {
            err_code = BLECB_MagneticChange(m_conn_handle, &m_cb, magnetic_logic_level);
            BLECB_CheckError(err_code);
            numsof100ticks_door = 0;
        }

        //NRF_LOG_INFO("pre %d\n", magnetic_task_pre_state);
        //NRF_LOG_INFO("logic %d\n", magnetic_logic_level);
        magnetic_task_pre_state = magnetic_logic_level;
        magnetic_flag = false;
    }
    /* lay trang thai cuoi cung neu co bug */
    if ((numsof100ticks_door > DOOR_MINIMUM_INTERVAL) && (numsof100ticks_door < DOOR_MAXIMUM_INTERVAL))
    {
        err_code = BLECB_MagneticChange(m_conn_handle, &m_cb, magnetic_logic_level);
        BLECB_CheckError(err_code);
        numsof100ticks_door = DOOR_MAXIMUM_INTERVAL + 1;
    }
}
void SENSOR_PIR_Task(void)
{
    ret_code_t err_code;

        if ((numsof100ticks_pir > PIR_TIMEOUT) && !pir_logic_level) // ko co chuyen dong trong PIR_TIMEOUT giay
        {
            pir_task_state = 0;
            //NRF_LOG_INFO("task 0");
        }
        if (pir_logic_level) // co chuyen dong
        {
            pir_task_state = 1;
            numsof100ticks_pir = 0;
            //NRF_LOG_INFO("task 1");
        }
        if (pir_task_pre_state != pir_task_state)
        {
            err_code = BLECB_PIRChange(m_conn_handle, &m_cb, pir_task_state);
            BLECB_CheckError(err_code);
        }
        //NRF_LOG_INFO("%d\n", numsof1000ticks_pir);
        // NRF_LOG_INFO("%d %d %d %d\n", pir_pre_state, pir_logic_level, pir_task_pre_state, pir_task_state);
        pir_task_pre_state = pir_task_state;
}

void SENSOR_TickCount(void *p_context) // 100 ms
{
    numsof100ticks_pir++;
    numsof100ticks_door++;
}

void SENSOR_CreateTimer(void)
{
    ret_code_t err_code;
    err_code = app_timer_create(&timer_systick_id,
                                APP_TIMER_MODE_REPEATED,
                                SENSOR_TickCount);
    APP_ERROR_CHECK(err_code);
    APP_ERROR_CHECK(app_timer_start(timer_systick_id, APP_TIMER_TICKS(100), NULL)); // ngat 100 ms
}

/**
 * @}
 */
