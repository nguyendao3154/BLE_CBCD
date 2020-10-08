/**
 ******************************************************************************
 * @file    BLE_spec.c
 * @author  Makipos Co.,LTD.
 * @version 1.0
 * @date    May 28, 2020
 * @brief   
 * @history
 * 
 *                      Revision History                                      *
 ****************************
 * Revision     Date            By              Description                         *
 ****************************
 * 1.0.0        28-May-2020     Nguyen Dao          create                                *

 ******************************************************************************/
/*******************************************************************************
 * Include
 ******************************************************************************/
#include "BLE_spec.h"
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"

uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; /**< Handle of the current connection. */
extern bool request_led_on;
uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;           /**< Advertising handle used to identify an advertising set. */
uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];            /**< Buffer for storing an encoded advertising set. */
uint8_t m_enc_scan_response_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX]; /**< Buffer for storing an encoded scan data. */
ble_gap_adv_data_t m_adv_data =
    {
        .adv_data =
            {
                .p_data = m_enc_advdata,
                .len = BLE_GAP_ADV_SET_DATA_SIZE_MAX},
        .scan_rsp_data =
            {
                .p_data = m_enc_scan_response_data,
                .len = BLE_GAP_ADV_SET_DATA_SIZE_MAX

            }};

extern ble_cb_t m_cb;
extern volatile uint8_t cell_percent;
extern volatile uint8_t pir_task_state;
extern volatile uint8_t magnetic_logic_level;

void BLE_GapParamsInit(void)
{
    ret_code_t err_code;
    ble_gap_conn_params_t gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

void BLE_GattInit(nrf_ble_gatt_t *p_gatt)
{
    ret_code_t err_code = nrf_ble_gatt_init(p_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}

void BLE_ServicesInit(ble_cb_t *p_cb)
{
    uint32_t err_code;
    ble_dis_init_t dis_init;
    ble_cb_init_t init = {0};

    // init.pir_write_handler = pir_write_handler;
    err_code = BLECB_Init(p_cb, &init);

    APP_ERROR_CHECK(err_code);

    memset(&dis_init, 0, sizeof(dis_init));

    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, (char *)MANUFACTURER_NAME);

    ble_srv_ascii_to_utf8(&dis_init.model_num_str, (char *)MODEL_NUMBER);

    ble_srv_ascii_to_utf8(&dis_init.serial_num_str, (char *)SERIAL_NUMBER);

    dis_init.dis_char_rd_sec = SEC_OPEN;

    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);
}

void BLE_OnConnParamsEvt(ble_conn_params_evt_t *p_evt)
{
    ret_code_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

void BLE_AdvertisingInit(ble_cb_t *p_cb)
{
    ret_code_t err_code;
    ble_advdata_t advdata;
    ble_advdata_t srdata;

    ble_uuid_t adv_uuids[] = {{CB_UUID_SERVICE, p_cb->uuid_type}};

    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance = true;
    advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    memset(&srdata, 0, sizeof(srdata));
    srdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    srdata.uuids_complete.p_uuids = adv_uuids;

    err_code = ble_advdata_encode(&advdata, m_adv_data.adv_data.p_data, &m_adv_data.adv_data.len);
    APP_ERROR_CHECK(err_code);

    err_code = ble_advdata_encode(&srdata, m_adv_data.scan_rsp_data.p_data, &m_adv_data.scan_rsp_data.len);
    APP_ERROR_CHECK(err_code);

    ble_gap_adv_params_t adv_params;

    // Set advertising parameters.
    memset(&adv_params, 0, sizeof(adv_params));

    adv_params.primary_phy = BLE_GAP_PHY_1MBPS;
    adv_params.duration = APP_ADV_DURATION;
    adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
    adv_params.p_peer_addr = NULL;
    adv_params.filter_policy = BLE_GAP_ADV_FP_ANY;
    adv_params.interval = APP_ADV_INTERVAL;

    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, &adv_params);
    APP_ERROR_CHECK(err_code);
    err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_adv_handle, 4);
    APP_ERROR_CHECK(err_code);
}

void BLE_ConnParamsErrorHandler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

void BLE_ConnParamsInit(void)
{
    ret_code_t err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail = false;
    cp_init.evt_handler = BLE_OnConnParamsEvt;
    cp_init.error_handler = BLE_ConnParamsErrorHandler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for starting advertising.
 */
void BLE_AdvertisingStart(void)
{
    ret_code_t err_code;

    err_code = sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
}

void BLE_EvtHandler(ble_evt_t const *p_ble_evt, void *p_context)
{
    ret_code_t err_code;
    ble_cb_t *p_cb = (ble_cb_t *)p_context;
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        NRF_LOG_INFO("Connected");
        m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

        err_code = BLECB_ADCChange(m_conn_handle, &m_cb, cell_percent);

        BLECB_CheckError(err_code);

        err_code = BLECB_PIRChange(m_conn_handle, &m_cb, pir_task_state);

        BLECB_CheckError(err_code);

        err_code = BLECB_MagneticChange(m_conn_handle, &m_cb, magnetic_logic_level);

        BLECB_CheckError(err_code);

        break;

    case BLE_GAP_EVT_DISCONNECTED:
        NRF_LOG_INFO("Disconnected");
        m_conn_handle = BLE_CONN_HANDLE_INVALID;
        BLE_AdvertisingStart();
        break;

    case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
        // Pairing not supported
        err_code = sd_ble_gap_sec_params_reply(m_conn_handle,
                                               BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP,
                                               NULL,
                                               NULL);
        APP_ERROR_CHECK(err_code);
        break;

    // case BLE_GATTS_EVT_WRITE:
    //     on_write(p_cb, p_ble_evt);
    //     break;

    case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
    {
        NRF_LOG_DEBUG("PHY update request.");
        ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
        err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
        APP_ERROR_CHECK(err_code);
    }
    break;

    case BLE_GATTS_EVT_SYS_ATTR_MISSING:
        // No system attributes have been stored.
        err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
        APP_ERROR_CHECK(err_code);
        break;

    case BLE_GATTC_EVT_TIMEOUT:
        // Disconnect on GATT Client timeout event.
        NRF_LOG_DEBUG("GATT Client Timeout.");
        err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                         BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
        break;

    case BLE_GATTS_EVT_TIMEOUT:
        // Disconnect on GATT Server timeout event.
        NRF_LOG_DEBUG("GATT Server Timeout.");
        err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                         BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
        break;

    default:
        // No implementation needed.
        break;
    }
}

void BLE_StackInit(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, BLE_EvtHandler, NULL);
}

/**
 * @}
 */
