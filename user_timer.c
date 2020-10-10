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


uint32_t g_systick;

APP_TIMER_DEF(timer_systick_id);

void TickCount(void *p_context) // 100 ms
{
    g_systick++;
}

void User_CreateTimer(void)
{
    ret_code_t err_code;
    err_code = app_timer_create(&timer_systick_id,
                                APP_TIMER_MODE_REPEATED,
                                TickCount);
    APP_ERROR_CHECK(err_code);
    APP_ERROR_CHECK(app_timer_start(timer_systick_id, APP_TIMER_TICKS(100), NULL)); // ngat 100 ms
}
