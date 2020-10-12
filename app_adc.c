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

#define SAMPLES_IN_BUFFER 3
#define PIR_NUMBER_CHANNEL 4

#define BAT_NUMBER_CHANNEL 3

#define LDR_NUMBER_CHANNEL 6

#define ADC_RESOLUTION 4095
#define TEN_TIMES_V_REF 6
#define HUNDRED_TIMES_ADC_GAIN_HARDWARE 18
#define ADC_GAIN_SOFTWARE 4

#define NUMBER_OF_CELL_ADC_POINT 4
uint16_t adc_time_send;
uint16_t pir_adc_value;
uint16_t ldr_adc_value;

bool is_cell_adc_ready_to_sent = true;

extern uint16_t m_conn_handle;                                          /**< Handle of the current connection. */
extern uint8_t m_adv_handle;                                            /**< Advertising handle used to identify an advertising set. */
extern uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];            /**< Buffer for storing an encoded advertising set. */
extern uint8_t m_enc_scan_response_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX]; /**< Buffer for storing an encoded scan data. */
extern ble_cb_t m_cb;
extern uint8_t pir_sensitivity;
extern uint8_t ldr_sensitivity;
extern bool is_ADC_initialized;

static nrf_saadc_value_t m_buffer[2][SAMPLES_IN_BUFFER];

uint16_t battery_adc_table[NUMBER_OF_CELL_ADC_POINT] = {615, 675, 830, 950};
uint8_t battery_percent_table[NUMBER_OF_CELL_ADC_POINT] = {0, 10, 90, 100};
volatile uint8_t cell_percent;
static uint16_t u16pinvalue;

/**
 * @brief Tat ADC driver
 */
void ADC_DeinitDriver(void)
{
    // NRF_LOG_INFO("Off");
    //nrf_drv_saadc_is_busy();
    nrf_drv_saadc_uninit();                                                     //Unintialize SAADC to disable EasyDMA and save power
    NRF_SAADC->INTENCLR = (SAADC_INTENCLR_END_Clear << SAADC_INTENCLR_END_Pos); //Disable the SAADC interrupt
    NVIC_ClearPendingIRQ(SAADC_IRQn);
    is_ADC_initialized = false;
}

void ADC_CallBack(nrf_drv_saadc_evt_t const *p_event)
{
    // NRF_LOG_INFO("callback");
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
        ret_code_t err_code;
        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLES_IN_BUFFER);
        APP_ERROR_CHECK(err_code);

        for (int i = 0; i < SAMPLES_IN_BUFFER; i++)
        {
            //NRF_LOG_INFO("%d\r\n", p_event->data.done.p_buffer[i]); //Print the SAADC result on UART
        }
        //NRF_LOG_INFO("%d\n",p_event->data.done.p_buffer[0]);
        /* (p_event->data.done.p_buffer[1]) la gia tri doc ADC  
         * 12 bit resolution
         * Vref = 0.6
         * Battery 3V
         * Gain 0.18
         */
        // u16pinvalue = (p_event->data.done.p_buffer[1]) * TEN_TIMES_V_REF * 100 * ADC_GAIN_SOFTWARE / HUNDRED_TIMES_ADC_GAIN_HARDWARE / ADC_RESOLUTION;
        u16pinvalue = (p_event->data.done.p_buffer[1]);

        pir_adc_value = (p_event->data.done.p_buffer[0]) * 100 / HUNDRED_TIMES_ADC_GAIN_HARDWARE;
        ldr_adc_value = (p_event->data.done.p_buffer[2]);
        //Clear the SAADC interrupt if set
        ADC_DeinitDriver(); // gui xong du lieu ADC, tat ADC driver de tiet kiem nang luong
    }
}
void ADC_Init(void)
{
    // NRF_LOG_INFO("init");
    ret_code_t err_code;
    nrf_drv_saadc_config_t saadc_config;
    nrf_saadc_channel_config_t channel_config_1;
    nrf_saadc_channel_config_t channel_config_2;
    nrf_saadc_channel_config_t channel_config_3;

    //Configure SAADC
    saadc_config.resolution = NRF_SAADC_RESOLUTION_12BIT;    //Set SAADC resolution to 12-bit. This will make the SAADC output values from 0 (when input voltage is 0V) to 2^12=2048 (when input voltage is 3.6V for channel gain setting of 1/6).
    saadc_config.oversample = NRF_SAADC_OVERSAMPLE_DISABLED; //Set oversample to disabled (will give unwanted output with scan mode).
    saadc_config.interrupt_priority = APP_IRQ_PRIORITY_LOW;  //Set SAADC interrupt to low priority.
    saadc_config.low_power_mode = true;
    //Initialize SAADC
    err_code = nrf_drv_saadc_init(&saadc_config, ADC_CallBack); //Initialize the SAADC with configuration and callback function. The application must then implement the saadc_callback function, which will be called when SAADC interrupt is triggered
    APP_ERROR_CHECK(err_code);

    //Configure SAADC channel
    channel_config_1.reference = NRF_SAADC_REFERENCE_VDD4;     //Set internal reference of fixed 0.6 volts
    channel_config_1.gain = NRF_SAADC_GAIN1_4;                 //Set input gain to 1/6. The maximum SAADC input voltage is then 0.6V/(1/6)=3.6V. The single ended input range is then 0V-3.6V
    channel_config_1.acq_time = NRF_SAADC_ACQTIME_10US;        //Set acquisition time. Set low acquisition time to enable maximum sampling frequency of 200kHz. Set high acquisition time to allow maximum source resistance up to 800 kohm, see the SAADC electrical specification in the PS.
    channel_config_1.mode = NRF_SAADC_MODE_SINGLE_ENDED;       //Set SAADC as single ended. This means it will only have the positive pin as input, and the negative pin is shorted to ground (0V) internally.
    channel_config_1.pin_p = NRF_SAADC_INPUT_AIN4;             //Select the input pin for the channel. AIN0 pin maps to physical pin P0.02.
    channel_config_1.pin_n = NRF_SAADC_INPUT_DISABLED;         //Since the SAADC is single ended, the negative pin is disabled. The negative pin is shorted to ground internally.
    channel_config_1.resistor_p = NRF_SAADC_RESISTOR_DISABLED; //Disable pullup resistor on the input pin
    channel_config_1.resistor_n = NRF_SAADC_RESISTOR_DISABLED;
    channel_config_1.burst = NRF_SAADC_BURST_DISABLED;

    err_code = nrf_drv_saadc_channel_init(BAT_NUMBER_CHANNEL, &channel_config_1);
    APP_ERROR_CHECK(err_code);

    //Configure SAADC channel
    channel_config_2.reference = NRF_SAADC_REFERENCE_INTERNAL; //Set internal reference of fixed 0.6 volts
    channel_config_2.gain = NRF_SAADC_GAIN1_4;                 //Set input gain to 1/6. The maximum SAADC input voltage is then 0.6V/(1/6)=3.6V. The single ended input range is then 0V-3.6V
    channel_config_2.acq_time = NRF_SAADC_ACQTIME_10US;        //Set acquisition time. Set low acquisition time to enable maximum sampling frequency of 200kHz. Set high acquisition time to allow maximum source resistance up to 800 kohm, see the SAADC electrical specification in the PS.
    channel_config_2.mode = NRF_SAADC_MODE_SINGLE_ENDED;       //Set SAADC as single ended. This means it will only have the positive pin as input, and the negative pin is shorted to ground (0V) internally.
    channel_config_2.pin_p = NRF_SAADC_INPUT_AIN3;             //Select the input pin for the channel. AIN0 pin maps to physical pin P0.02.
    channel_config_2.pin_n = NRF_SAADC_INPUT_DISABLED;         //Since the SAADC is single ended, the negative pin is disabled. The negative pin is shorted to ground internally.
    channel_config_2.resistor_p = NRF_SAADC_RESISTOR_DISABLED; //Disable pullup resistor on the input pin
    channel_config_2.resistor_n = NRF_SAADC_RESISTOR_DISABLED; //Disable pulldown resistor on the input pin
    channel_config_2.burst = NRF_SAADC_BURST_DISABLED;

    err_code = nrf_drv_saadc_channel_init(PIR_NUMBER_CHANNEL, &channel_config_2);
    APP_ERROR_CHECK(err_code);

    //Configure SAADC channel
    channel_config_3.reference = NRF_SAADC_REFERENCE_INTERNAL; //Set internal reference of fixed 0.6 volts
    channel_config_3.gain = NRF_SAADC_GAIN1_4;                 //Set input gain to 1/6. The maximum SAADC input voltage is then 0.6V/(1/6)=3.6V. The single ended input range is then 0V-3.6V
    channel_config_3.acq_time = NRF_SAADC_ACQTIME_10US;        //Set acquisition time. Set low acquisition time to enable maximum sampling frequency of 200kHz. Set high acquisition time to allow maximum source resistance up to 800 kohm, see the SAADC electrical specification in the PS.
    channel_config_3.mode = NRF_SAADC_MODE_SINGLE_ENDED;       //Set SAADC as single ended. This means it will only have the positive pin as input, and the negative pin is shorted to ground (0V) internally.
    channel_config_3.pin_p = NRF_SAADC_INPUT_AIN6;             //Select the input pin for the channel. AIN0 pin maps to physical pin P0.02.
    channel_config_3.pin_n = NRF_SAADC_INPUT_DISABLED;         //Since the SAADC is single ended, the negative pin is disabled. The negative pin is shorted to ground internally.
    channel_config_3.resistor_p = NRF_SAADC_RESISTOR_DISABLED; //Disable pullup resistor on the input pin
    channel_config_3.resistor_n = NRF_SAADC_RESISTOR_DISABLED; //Disable pulldown resistor on the input pin
    channel_config_3.burst = NRF_SAADC_BURST_DISABLED;

    err_code = nrf_drv_saadc_channel_init(LDR_NUMBER_CHANNEL, &channel_config_3);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_buffer_convert(m_buffer[0], SAMPLES_IN_BUFFER); //Set SAADC buffer 1. The SAADC will start to write to this buffer
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_buffer_convert(m_buffer[1], SAMPLES_IN_BUFFER); //Set SAADC buffer 2. The SAADC will write to this buffer when buffer 1 is full. This will give the applicaiton time to process data in buffer 1.
    APP_ERROR_CHECK(err_code);
    nrf_drv_saadc_sample();

    is_ADC_initialized = true;
}

void cell_calculate_and_send(void)
{
    ret_code_t err_code;
    for (int i = 0; i < NUMBER_OF_CELL_ADC_POINT - 1; i++)
    {
        if (u16pinvalue > battery_adc_table[i] && u16pinvalue < battery_adc_table[i + 1])
        {
            cell_percent = (u16pinvalue - battery_adc_table[i]) * (battery_percent_table[i + 1] - battery_percent_table[i]) / (battery_adc_table[i + 1] - battery_adc_table[i]) + battery_percent_table[i];
        }
        if (u16pinvalue > battery_adc_table[NUMBER_OF_CELL_ADC_POINT - 1])
        {
            cell_percent = 100;
        }
        if (u16pinvalue < battery_adc_table[0])
        {
            cell_percent = 0;
        }
    }

    if (is_cell_adc_ready_to_sent == true)
    {
        NRF_LOG_INFO("%d", cell_percent);
        err_code = BLECB_ADCChange(m_conn_handle, &m_cb, cell_percent);
        is_cell_adc_ready_to_sent = false;
        BLECB_CheckError(err_code);
    }
}

void ADC_Task(void)
{

    if (is_ADC_initialized)
    {

        //NRF_LOG_INFO("%d", u16pinvalue);
        //   NRF_LOG_INFO("%d",ldr_adc_value);
        adc_time_send++;
        if (adc_time_send % 1000 == 0)
        {
            is_cell_adc_ready_to_sent = true;
        }
        cell_calculate_and_send();
    }
}

/**
 * @}
 */
