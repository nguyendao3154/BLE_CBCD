/**
 ******************************************************************************
 * @file    app_interrupt.h
 * @author  Makipos Co.,LTD.
 * @version 1.0
 * @date    May 28, 2020
 * @brief   
 * @history
 ******************************************************************************/

#ifndef APP_INTERRUPT_H
#define APP_INTERRUPT_H

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

#define TU_PIN 13
#define OUT1_PIN 14
#define OUT2_PIN 15

#define PIR_TIMEOUT 10       

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void in_tu_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

void tu_interrupt_init(void);

void in_out1_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

void out1_interrupt_init(void);

void in_out2_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

void out2_interrupt_init(void);

void interrupt_init(void);

void task_tu(void);

void task_chuyendong(void);

void create_sensor_timer(void);

#endif /* APP_INTERRUPT_H */
