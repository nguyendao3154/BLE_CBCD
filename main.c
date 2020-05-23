#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "BLE_spec.h"
#include "cambien_service.h"
#include "nrf_drv_gpiote.h"
#include "app_adc.h"

#define TU_PIN 3
#define OUT1_PIN 9
#define OUT2_PIN 10

#define PIR_TIMEOUT 10       // PIR timeout 10sadc_flag
APP_TIMER_DEF(timer_systick_id);
bool m_saadc_initialized = false;
volatile uint8_t pin_8bit_value;

NRF_BLE_GATT_DEF(m_gatt); /**< GATT module instance. */
ble_cb_t m_cb;

uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; /**< Handle of the current connection. */

uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;           /**< Advertising handle used to identify an advertising set. */
uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];            /**< Buffer for storing an encoded advertising set. */
uint8_t m_enc_scan_response_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX]; /**< Buffer for storing an encoded scan data. */
bool adc_ready = false;

/**@brief Struct that contains pointers to the encoded advertising data. */
uint32_t sys_tick;
uint8_t pir_state;
ble_gap_adv_data_t m_adv_data;

bool TU_FLAG = false;
uint8_t tu_logic_level;
void in_tu_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    TU_FLAG = true;
    if (!nrf_gpio_pin_read(TU_PIN))
    {
        tu_logic_level = 1;
        //NRF_LOG_INFO("Co nam cham\r\n");
    }
    else
    {
        tu_logic_level = 0;
        //NRF_LOG_INFO("Xa nam cham\r\n");
    }
}

void tu_interrupt_init()
{
    nrf_drv_gpiote_in_config_t tu_config;
    tu_config.pull = NRF_GPIO_PIN_PULLUP;
    tu_config.sense = NRF_GPIOTE_POLARITY_TOGGLE;
    tu_config.is_watcher = false;
    tu_config.hi_accuracy = false;
    nrf_drv_gpiote_in_init(TU_PIN,
                           &tu_config,
                           in_tu_handler);
    nrf_drv_gpiote_in_event_enable(TU_PIN, true);
}
void in_out1_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (nrf_gpio_pin_read(OUT1_PIN))
    {
        pir_state = 1;
        NRF_LOG_INFO("CD 1\r\n");
    }
    else
    {
        pir_state = 0;
        NRF_LOG_INFO("CD 0\r\n");
    }
}

void out1_interrupt_init()
{
    nrf_drv_gpiote_in_config_t out1_config;
    //out1_config.pull = NRF_GPIO_PIN_PULLUP;
    out1_config.pull = NRF_GPIO_PIN_PULLDOWN;
    out1_config.sense = NRF_GPIOTE_POLARITY_TOGGLE;
    out1_config.is_watcher = false;
    out1_config.hi_accuracy = false;
    nrf_drv_gpiote_in_init(OUT1_PIN,
                           &out1_config,
                           in_out1_handler);
    nrf_drv_gpiote_in_event_enable(OUT1_PIN, true);
}
void in_out2_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (nrf_gpio_pin_read(OUT2_PIN))
    {
        pir_state = 1;
        NRF_LOG_INFO("CD 1\r\n");
    }
    else
    {
        pir_state = 0;
        NRF_LOG_INFO("CD 0\r\n");
    }
}

void out2_interrupt_init()
{
    nrf_drv_gpiote_in_config_t out2_config;
    // out2_config.pull = NRF_GPIO_PIN_PULLUP;
    out2_config.pull = NRF_GPIO_PIN_PULLDOWN;
    out2_config.sense = NRF_GPIOTE_POLARITY_TOGGLE;
    out2_config.is_watcher = false;
    out2_config.hi_accuracy = false;
    nrf_drv_gpiote_in_init(OUT2_PIN,
                           &out2_config,
                           in_out2_handler);
    nrf_drv_gpiote_in_event_enable(OUT2_PIN, true);
}
void interrupt_init()
{
    nrf_drv_gpiote_init();
    //tu_interrupt_init();
    out1_interrupt_init();
    out2_interrupt_init();
}
void systick_handle(void *p_context)
{
    sys_tick++;
}

static void timers_init(void)
{
    // Initialize timer module, making it use the scheduler
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&timer_systick_id,
                                APP_TIMER_MODE_REPEATED,
                                systick_handle);
    APP_ERROR_CHECK(err_code);
    APP_ERROR_CHECK(app_timer_start(timer_systick_id, APP_TIMER_TICKS(1000), NULL));
}

void task_tu()
{
    ret_code_t err_code;
    if (TU_FLAG)
    {

        err_code = ble_cb_tu_change(m_conn_handle, &m_cb, tu_logic_level);

        check_error_ble(err_code);
        TU_FLAG = false;
    }
}
uint8_t pir_pre_state = 0, task_state = 0, task_pre_state = 0;

void task_chuyendong()
{
    ret_code_t err_code;

    if ((sys_tick > PIR_TIMEOUT) && !pir_state) // ko co gi
    {
        task_state = 2;
         NRF_LOG_INFO("task 2");
    }
    if ((sys_tick > PIR_TIMEOUT) && pir_state) // co chuyen dong
    {
        task_state = 1;
         NRF_LOG_INFO("task 1");
    }
    if (task_pre_state != task_state)
    {
        err_code = ble_cb_chuyendong_change(m_conn_handle, &m_cb, pir_state);
        check_error_ble(err_code);
    }
    if (pir_state)
    {
        sys_tick = 0;
    }
     //NRF_LOG_INFO("%d\n", sys_tick);
    // NRF_LOG_INFO("%d %d %d %d\n", pir_pre_state, pir_state, task_pre_state, task_state);
    pir_pre_state = pir_state;
    task_pre_state = task_state;
}
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

static void idle_state_handle(void)
{

    nrf_pwr_mgmt_run();
}
void task_adc(void)
{
    ret_code_t err_code;
    nrf_drv_saadc_sample();
    // check_status(adc_ready);
    if (m_saadc_initialized)
    {
        // NRF_LOG_INFO("abc");
        err_code = ble_cb_ADC_change(m_conn_handle, &m_cb, pin_8bit_value);
        // NRF_LOG_INFO("ab");
        check_error_ble(err_code);
        // NRF_LOG_INFO("abc");
        turn_off_saadc_driver();   
    }
}
int main(void)
{
    // Initialize.
    set_BLE_para();
    //log_init();
    timers_init();
    create_ADC_timer();
    power_management_init();
    interrupt_init();
    saadc_init();
    ble_stack_init();
    gap_params_init();
    gatt_init(&m_gatt);
    services_init(&m_cb);
    advertising_init(&m_cb);
    conn_params_init();
    nrf_drv_saadc_sample();
    // Start execution.
    //check_error_ble(ble_cb_ADC_change(m_conn_handle, &m_cb, get_adc_value()));

    NRF_LOG_INFO("CBCD started.");
    advertising_start();

    // Enter main loop.
    for (;;)
    {

        //task_tu();
        task_chuyendong(); 
        task_adc();
        //NRF_LOG_FLUSH();

        idle_state_handle();
    }
}

/**
 * @}
 */
