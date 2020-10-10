/**
 ******************************************************************************
 * @file    user_timer.c
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
#include "user_timer.h"

#define ADC_TIME_SCAN 100 // ADC quet 1 ngay 1 lan

uint32_t g_systick;
bool is_ADC_initialized = false;

APP_TIMER_DEF(timer_systick_id);

void TimerHandle(void *p_context) // 100 ms
{
    g_systick++;
		if (!is_ADC_initialized)
    {
        ADC_Init(); //Initialize the SAADC. In the case when SAADC_SAMPLES_IN_BUFFER > 1 then we only need to initialize the SAADC when the the buffer is empty.
    }
}

void User_CreateTimer(void)
{
    ret_code_t err_code;
    err_code = app_timer_create(&timer_systick_id,
                                APP_TIMER_MODE_REPEATED,
                                TimerHandle);
    APP_ERROR_CHECK(err_code);
    APP_ERROR_CHECK(app_timer_start(timer_systick_id, APP_TIMER_TICKS(100), NULL)); // ngat 100 ms
}
