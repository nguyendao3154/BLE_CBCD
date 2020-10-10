
/**
 ******************************************************************************
 * @file    cambien_service.h
 * @author  Makipos Co.,LTD. Nguyen DD
 * @version 1.0
 * @date    October 10, 2020
 * @brief   
 * @history
 ******************************************************************************/
#ifndef APP_LDR_H
#define APP_LDR_H

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
#include "app_adc.h"
#include "app_sensor.h"

typedef enum _ldr_state
{
    LDR_State_1 = 1,
    LDR_State_2,
    LDR_State_3,
    LDR_State_4,
    LDR_State_5,
    LDR_State_6,
    LDR_State_7,
    LDR_State_8

}ldr_state_t;

typedef struct{
    uint8_t current_logic;
    uint8_t previous_logic;
    uint8_t current_state;
    uint8_t previous_state;
    uint32_t current_tick;
}ldr_param_t;



void LDR_Process(void);

#endif
