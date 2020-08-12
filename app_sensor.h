/**
 ******************************************************************************
 * @file    app_interrupt.h
 * @author  Makipos Co.,LTD.
 * @version 1.0
 * @date    May 28, 2020
 * @brief   
 * @history
 ******************************************************************************/

#ifndef APP_SENSOR_H
#define APP_SENSOR_H

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

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
#define MAGNETIC_PIN 4
#define PIR_OUT1_PIN 9
#define PIR_OUT2_PIN 10

void SENSOR_MagneticGetInitialValue(void);

void SENSOR_MagneticHandle(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

void SENSOR_MagneticInit(void);

void SENSOR_PIR_Out1Handle(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

void SENSOR_PIR_OUT1Init(void);

void SENSOR_PIR_Out2Handle(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

void SENSOR_PIR_OUT2Init(void);

void SENSOR_InterruptInit(void);

void SENSOR_MagneticTask(void);

void SENSOR_PIR_Task(void);

void SENSOR_CreateTimer(void);

void SENSOR_TickCount(void *p_context);

#endif /* APP_SENSOR_H */

/**
 * @}
 */
