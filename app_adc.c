/**
 ******************************************************************************
 * @file    app_adc.c
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
#include "app_adc.h"

#define SAMPLES_IN_BUFFER 2
#define NUMBERS_OF_CHANNEL 2
#define PIR_NUMBER_CHANNEL 0
#define AIN_PIR_CHANNEL NRF_SAADC_INPUT_AIN0
#define BAT_NUMBER_CHANNEL 3
#define AIN_BAT_CHANNEL NRF_SAADC_INPUT_AIN3
#define ADC_TIME_SCAN 5000 // ADC quet 1 ngay 1 lan

#define ADC_RESOLUTION 1024
#define TEN_TIMES_V_REF 6
#define HUNDRED_TIMES_ADC_GAIN_HARDWARE 18
bool is_ADC_initialized;
volatile uint8_t u8pinvalue;
extern uint16_t m_conn_handle;                                          /**< Handle of the current connection. */
extern uint8_t m_adv_handle;                                            /**< Advertising handle used to identify an advertising set. */
extern uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];            /**< Buffer for storing an encoded advertising set. */
extern uint8_t m_enc_scan_response_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX]; /**< Buffer for storing an encoded scan data. */
extern ble_cb_t m_cb;

APP_TIMER_DEF(m_adc_id);
static nrf_saadc_value_t m_buffer[SAMPLES_IN_BUFFER][NUMBERS_OF_CHANNEL];
static uint32_t u32PinValue;

/**
 * @brief Tat ADC driver
 */
void ADC_DeinitDriver(void)
{
    // NRF_LOG_INFO("Off");
    nrf_drv_saadc_uninit();                                                     //Unintialize SAADC to disable EasyDMA and save power
    NRF_SAADC->INTENCLR = (SAADC_INTENCLR_END_Clear << SAADC_INTENCLR_END_Pos); //Disable the SAADC interrupt
    NVIC_ClearPendingIRQ(SAADC_IRQn);
    is_ADC_initialized = false;
}

void ADC_CallBack(nrf_drv_saadc_evt_t const *p_event)
{
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
        ret_code_t err_code;
        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLES_IN_BUFFER);
        APP_ERROR_CHECK(err_code);

        for (int i = 0; i < SAMPLES_IN_BUFFER; i++)
        {
            NRF_LOG_INFO("%d\r\n", p_event->data.done.p_buffer[i]);                                           //Print the SAADC result on UART
        }
        //NRF_LOG_INFO("%d\n",p_event->data.done.p_buffer[0]);
        /* (p_event->data.done.p_buffer[0]) la gia tri doc ADC  
         * 10 bit resolution
         * Vref = 0.6
         * Battery 3V
         * Gain 0.18
         */
        u32PinValue = (p_event->data.done.p_buffer[0])*TEN_TIMES_V_REF*100/HUNDRED_TIMES_ADC_GAIN_HARDWARE/ADC_RESOLUTION; 
        u8pinvalue = (uint8_t)u32PinValue;
        // NRF_LOG_INFO("%d\r\n", u32PinValue);
        //Clear the SAADC interrupt if set
    }
}
void ADC_Init(void)
{
    // NRF_LOG_INFO("init");
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel1_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(AIN_BAT_CHANNEL);
    channel1_config.gain = NRF_SAADC_GAIN1;
    nrf_saadc_channel_config_t channel2_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(AIN_PIR_CHANNEL);

    err_code = nrf_drv_saadc_init(NULL, ADC_CallBack);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_channel_init(BAT_NUMBER_CHANNEL, &channel1_config);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_channel_init(PIR_NUMBER_CHANNEL, &channel2_config);
    APP_ERROR_CHECK(err_code);

     err_code = nrf_drv_saadc_buffer_convert(m_buffer[0],SAMPLES_IN_BUFFER);    //Set SAADC buffer 1. The SAADC will start to write to this buffer
    APP_ERROR_CHECK(err_code);
    
    err_code = nrf_drv_saadc_buffer_convert(m_buffer[1],SAMPLES_IN_BUFFER);    //Set SAADC buffer 2. The SAADC will write to this buffer when buffer 1 is full. This will give the applicaiton time to process data in buffer 1.
    APP_ERROR_CHECK(err_code);
    is_ADC_initialized = true;
}
/**
 * @brief bat ADC bang timer
 */
void ADC_HandleTimer(void *p_context)
{
    // NRF_LOG_INFO("handle");
    if (!is_ADC_initialized)
    {
        ADC_Init(); //Initialize the SAADC. In the case when SAADC_SAMPLES_IN_BUFFER > 1 then we only need to initialize the SAADC when the the buffer is empty.
    }
}
/**
 * @brief tao timer
 * ADC_TIME_SCAN: thoi gian giua moi lan ngat
 * ADC_HandleTimer: ham callback timer
 */
void ADC_CreateTimer(void)
{
    ret_code_t err_code;
    // Create timers
    err_code = app_timer_create(&m_adc_id,
                                APP_TIMER_MODE_REPEATED,
                                ADC_HandleTimer);
    APP_ERROR_CHECK(err_code);
    APP_ERROR_CHECK(app_timer_start(m_adc_id, APP_TIMER_TICKS(ADC_TIME_SCAN), NULL));
}

void ADC_Task(void)
{
    ret_code_t err_code;
    nrf_drv_saadc_sample();
    if (is_ADC_initialized)
    {
        err_code = BLECB_ADCChange(m_conn_handle, &m_cb, u8pinvalue);
        BLECB_CheckError(err_code);
        ADC_DeinitDriver();                              // gui xong du lieu ADC, tat ADC driver de tiet kiem nang luong
    }
}

/**
 * @}
 */
