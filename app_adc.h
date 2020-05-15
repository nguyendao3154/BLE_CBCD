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

void turn_off_saadc_driver(void);

void saadc_callback(nrf_drv_saadc_evt_t const *p_event);

void saadc_init(void);

void create_ADC_timer(void);

void adc_handle_timer(void *p_context);

#endif
