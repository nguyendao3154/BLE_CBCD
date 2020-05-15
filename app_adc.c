#include "app_adc.h"

void turn_off_saadc_driver(void)
{
    nrf_drv_saadc_uninit();                                                     //Unintialize SAADC to disable EasyDMA and save power
    NRF_SAADC->INTENCLR = (SAADC_INTENCLR_END_Clear << SAADC_INTENCLR_END_Pos); //Disable the SAADC interrupt
    NVIC_ClearPendingIRQ(SAADC_IRQn);
    m_saadc_initialized = false;
}
void saadc_callback(nrf_drv_saadc_evt_t const *p_event)
{
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
        ret_code_t err_code; 
        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLES_IN_BUFFER);
        APP_ERROR_CHECK(err_code);
        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLES_IN_BUFFER);
        APP_ERROR_CHECK(err_code);
        // pin_value = (p_event->data.done.p_buffer[0]) * 12 * 11 / 1024;
        //NRF_LOG_INFO("%d\n",p_event->data.done.p_buffer[0]);
        pir_analog_value = (p_event->data.done.p_buffer[0]) * 50 / 205; //
        pin_8bit_value = (uint8_t)pir_analog_value;
        NRF_LOG_INFO("%d\r\n", pir_analog_value);
        //Clear the SAADC interrupt if set
        
    }
}
void saadc_init(void)
{
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel1_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(AIN_BAT_CHANNEL);
    channel1_config.gain = NRF_SAADC_GAIN1_2;
    // nrf_saadc_channel_config_t channel2_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(AIN_BAT_CHANNEL);

    err_code = nrf_drv_saadc_init(NULL, saadc_callback);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_channel_init(BAT_NUMBER_CHANNEL, &channel1_config);
    APP_ERROR_CHECK(err_code);

    // err_code = nrf_drv_saadc_channel_init(1, &channel2_config);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_buffer_convert(m_buffer, SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);
}
uint8_t get_adc_value()
{
    return pin_8bit_value;
}

void adc_handle_timer(void *p_context)
{

    if (!m_saadc_initialized)
    {
        saadc_init(); //Initialize the SAADC. In the case when SAADC_SAMPLES_IN_BUFFER > 1 then we only need to initialize the SAADC when the the buffer is empty.
    }
    m_saadc_initialized = true;
}

void create_ADC_timer(void)
{
    ret_code_t err_code;
    // Create timers
    err_code = app_timer_create(&m_adc_id,
                                APP_TIMER_MODE_REPEATED,
                                adc_handle_timer);
    APP_ERROR_CHECK(err_code);
    APP_ERROR_CHECK(app_timer_start(m_adc_id, APP_TIMER_TICKS(ADC_TIME_SCAN), NULL));
}

bool check_status(void)
{
    return m_saadc_initialized;
}

