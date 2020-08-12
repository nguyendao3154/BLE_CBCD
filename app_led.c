#include "app_led.h"

#define LED_PIN 20
#define LED_ON_TIME_DEFAUT 30              // 3s

bool request_led_on = false;
//extern uint32_t numsof100ticks_led;

void LED_Config(void)
{
    nrf_gpio_cfg_output(LED_PIN);
    nrf_gpio_pin_clear(LED_PIN);
}

void LED_Turn_on(void)
{
    nrf_gpio_pin_clear(LED_PIN);
}

void LED_Turn_off(void)
{
    nrf_gpio_pin_set(LED_PIN);
}
void LED_process(void)
{
    if (request_led_on)
    {
        LED_Turn_on();
    }
    if (!request_led_on)
    {
        LED_Turn_off();
    }
}
void LED_Task(void)
{
    // if (numsof100ticks_led == LED_ON_TIME_DEFAUT)
    // {
    //     request_led_on = false;
    // }

    LED_process();
}
