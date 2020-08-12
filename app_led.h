 /******************************************************************************
 * @file    app_led.h
 * @author  Makipos Co.,LTD.
 * @version 1.0
 * @date    May 28, 2020
 * @brief   
 * @history
 ******************************************************************************/
#ifndef APP_LED_H__
#define APP_LED_H__
#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "app_timer.h"

void LED_Config(void);

void LED_Turn_on(void);

void LED_Turn_off(void);

void LED_Task(void);

#endif
