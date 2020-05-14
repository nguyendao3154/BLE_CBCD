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

#define SAMPLES_IN_BUFFER 1
#define AIN_PIR_CHANNEL NRF_SAADC_INPUT_AIN0
#define BAT_NUMBER_CHANNEL 2
#define AIN_BAT_CHANNEL NRF_SAADC_INPUT_AIN2
#define ADC_TIME_SCAN 100000 // ADC quet 1 ngay 1 lan

APP_TIMER_DEF(m_adc_id);
static nrf_saadc_value_t m_buffer[SAMPLES_IN_BUFFER];
static uint8_t pin_8bit_value;
static uint32_t pin_value;
static uint32_t pir_analog_value;
bool adc_flag = false;
static bool m_saadc_initialized = false;

void turn_off_saadc_driver(void);

void saadc_callback(nrf_drv_saadc_evt_t const *p_event);

void saadc_init(void);

uint8_t get_adc_value(void);

void create_ADC_timer(void);

void adc_handle_timer(void *p_context);

bool check_status(void);

void delete_flag(void);
#endif
