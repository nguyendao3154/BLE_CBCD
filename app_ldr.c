/**
 ******************************************************************************
 * @file    app_ldr.c
 * @author  Makipos Co.,LTD.
 * @version 1.0
 * @date    October 10, 2020
 * @brief
 * @history
 *
 *                      Revision History                                      *
 ****************************
 * Revision     Date            By              Description                         *
 ****************************
 * 1.0.0        10-October-2020     Nguyen Dao          create                                *

 ******************************************************************************/
/*******************************************************************************
  * Include
  ******************************************************************************/
#include "app_ldr.h"

#define LDR_INTERVAL_SCALE 750
#define LDR_OFFSET_VALUE 10
#define MIN_LDR_INV 10 //1s
#define NUMBER_OF_LDR_POINT 9

extern uint16_t ldr_adc_value;
extern uint8_t ldr_sensitivity;
extern uint32_t g_systick;
extern uint16_t m_conn_handle;                                          /**< Handle of the current connection. */
extern uint8_t m_adv_handle;                                            /**< Advertising handle used to identify an advertising set. */
extern uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];            /**< Buffer for storing an encoded advertising set. */
extern uint8_t m_enc_scan_response_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX]; /**< Buffer for storing an encoded scan data. */
extern ble_cb_t m_cb;

uint8_t ldr_BleMessage;
uint16_t ldr_offset[NUMBER_OF_LDR_POINT] = {0, 550, 1000, 1450, 1900, 2350, 2800, 3250, 4096};

ldr_param_t ldr_state =
    {
        .current_logic = 0,
        .current_state = 0

};

void LDR_receive()
{
}
void ldr_SendToBle(void)
{
    ret_code_t err_code;
    err_code = BLECB_LDRChange(m_conn_handle, &m_cb, ldr_state.current_state);
    BLECB_CheckError(err_code);
}

void LDR_Process(void)
{
    for (int i = 0; i < NUMBER_OF_LDR_POINT-1; i++)
    {
        if (ldr_adc_value > ldr_offset[i] && ldr_adc_value < ldr_offset[i + 1])
        {
            ldr_state.current_logic = i + 1;
            // NRF_LOG_INFO("%d",ldr_state.current_logic);
        }
    }
    if (ldr_state.current_logic != ldr_state.previous_logic)
    {
        ldr_state.current_tick = g_systick;
    }
    if (ldr_state.current_logic == ldr_state.previous_logic)
    {
        if (g_systick - ldr_state.current_tick >= MIN_LDR_INV)
        {
            // for (int i = 0; i < NUMBER_OF_LDR_POINT-1; i++)
            // {
                // if (ldr_state.current_logic > ldr_offset[i] && ldr_state.current_logic < ldr_offset[i + 1])
                // {
                //     ldr_state.current_state = i + 1;
                //     NRF_LOG_INFO("state %d",ldr_state.current_state);
                // }
                ldr_state.current_state = ldr_state.current_logic;
            // }
        }
    }
    if (ldr_state.current_state != ldr_state.previous_state)
    {
        ldr_SendToBle();
        //NRF_LOG_INFO("hello");
    }
    ldr_state.previous_logic = ldr_state.current_logic;
    ldr_state.previous_state = ldr_state.current_state;
}
