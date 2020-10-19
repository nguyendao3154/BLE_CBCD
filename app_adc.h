/**
 ******************************************************************************
 * @file    app_adc.h
 * @author  Makipos Co.,LTD.
 * @version 1.0
 * @date    May 28, 2020
 * @brief   
 * @history
 ******************************************************************************/
#ifndef APP_ADC_H__
#define APP_ADC_H__
#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "app_error.h"
#include "nrf_drv_saadc.h"
#include "app_timer.h"
#include "BLE_spec.h"
#include "app_sensor.h"

#define LINEAR_PAM_A 171
#define LINEAR_PAM_B 3071
/* 171*Vdd*10 + 3071 = k*PIR_INV */
typedef struct linear_param{
    uint16_t Vdd_10;
    uint16_t k;
    uint16_t pir_scale;
}linear_param_t;

void ADC_DeinitDriver(void);

void ADC_CallBack(nrf_drv_saadc_evt_t const *p_event);

void ADC_Init(void);

void ADC_Task(void);
#endif

/**
 * @}
 */
