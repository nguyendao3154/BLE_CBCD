/**
 ******************************************************************************
 * @file    user_timer.h
 * @author  Makipos Co.,LTD. Nguyen DD
 * @version 1.0
 * @date    May 28, 2020
 * @brief   
 * @history
 ******************************************************************************/

#ifndef USER_TIMER_H
#define USER_TIMER_H

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "app_error.h"
#include "nrf_drv_gpiote.h"
#include "app_timer.h"
#include "BLE_spec.h"
#include "app_led.h"
#include "app_adc.h"

void TimerHandle(void *p_context);

void User_CreateTimer(void);

#endif
