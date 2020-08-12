#include "app_led.h"

#define LED_PIN 20
#define LED_ON_TIME_DEFAUT 30              // 3s

bool request_led_on = false;
extern uint32_t sensor_ticks;
uint32_t led_current_time;

void LED_Config(void)
{
    nrf_gpio_cfg_output(LED_PIN);
    LED_Turn_on();
}

void LED_Turn_on(void)
{
    nrf_gpio_pin_clear(LED_PIN);
}

void LED_Turn_off(void)
{
    nrf_gpio_pin_set(LED_PIN);
}

void LED_Task(void)
{
    if (sensor_ticks - led_current_time > LED_ON_TIME_DEFAUT)
    {
        LED_Turn_off();       
    }  
    if(request_led_on)
    {
        led_current_time = sensor_ticks;
        LED_Turn_on();
        request_led_on = false;
    }
}
