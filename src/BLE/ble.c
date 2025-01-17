
#include "../../inc/Configurations/Configurations.h"
#include "../../inc/BLE/ble.h"

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

#define MAX_ADVERTISE_PACKET 250
#define BT_ADVERTISE_INTERVAL_MIN 0x20
#define BT_ADVERTISE_INTERVAL_MAX 0x40

#define BT_LE_ADV_CONNECT BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_TX_POWER, BT_ADVERTISE_INTERVAL_MIN, BT_ADVERTISE_INTERVAL_MAX, NULL)

// #define BT_LE_ADV_CONNECT_LE_CODED BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_CODED, BT_ADVERTISE_INTERVAL_MIN, BT_ADVERTISE_INTERVAL_MAX, NULL)

uint8_t adv_packet_no = 0;
uint8_t ble_advertise_status = BLE_NOT_ADVERTISING;

static struct bt_le_ext_adv *adv;

static void set_tx_power(uint8_t handle_type, uint16_t handle, int8_t tx_pwr_lvl)
{
    struct bt_hci_cp_vs_write_tx_power_level *cp;
    struct bt_hci_rp_vs_write_tx_power_level *rp;
    struct net_buf *buf, *rsp = NULL;
    int err;

    buf = bt_hci_cmd_create(BT_HCI_OP_VS_WRITE_TX_POWER_LEVEL, sizeof(*cp));
    if (!buf)
    {
        printk("Unable to allocae command buffer\n");
        return;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(handle);
    cp->handle_type = handle_type;
    cp->tx_power_level = tx_pwr_lvl;

    err = bt_hci_cmd_send_sync(BT_HCI_OP_VS_WRITE_TX_POWER_LEVEL,
                               buf, &rsp);
    if (err)
    {
        printk("Set Tx power err: %d\n", err);
        return;
    }

    rp = (void *)rsp->data;
    printk("Actual Tx Power: %d\n", rp->selected_tx_power);

    net_buf_unref(rsp);
}

static void get_tx_power(uint8_t handle_type, uint16_t handle, int8_t *tx_pwr_lvl)
{
    struct bt_hci_cp_vs_read_tx_power_level *cp;
    struct bt_hci_rp_vs_read_tx_power_level *rp;
    struct net_buf *buf, *rsp = NULL;
    int err;

    *tx_pwr_lvl = 0xFF;
    buf = bt_hci_cmd_create(BT_HCI_OP_VS_READ_TX_POWER_LEVEL,
                            sizeof(*cp));
    if (!buf)
    {
        printk("Unable to allocate command buffer\n");
        return;
    }

    cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(handle);
    cp->handle_type = handle_type;

    err = bt_hci_cmd_send_sync(BT_HCI_OP_VS_READ_TX_POWER_LEVEL,
                               buf, &rsp);
    if (err)
    {
        printk("Read Tx power err: %d\n", err);
        return;
    }

    rp = (void *)rsp->data;
    *tx_pwr_lvl = rp->tx_power_level;

    net_buf_unref(rsp);
}

uint8_t sensor_data[] = {
    0x4C, 0x00, // 0x59 00 nordic, 0x4c 00 apple
    0x02, 0x15,
    0x1A, 0xCC, 0x1B, 0x16, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
    0x00, 0x00, 0x00, 0x00, 0x00};

static const struct bt_data sensor[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR | BT_LE_AD_GENERAL),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, sensor_data, sizeof(sensor_data)),
};
#ifdef CONFIG_ENABLE_LE_CODED
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_HRS_VAL),
                  BT_UUID_16_ENCODE(BT_UUID_BAS_VAL),
                  BT_UUID_16_ENCODE(BT_UUID_DIS_VAL)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN)};

#endif

void get_public_mac_address(void)
{
    struct net_buf *rsp;
    struct bt_hci_rp_read_bd_addr *rp;
    int err;

    err = bt_hci_cmd_send_sync(BT_HCI_OP_READ_BD_ADDR, NULL, &rsp);
    if (err)
    {
        printk("Failed to get public address (err %d)\n", err);
        return;
    }

    rp = (struct bt_hci_rp_read_bd_addr *)rsp->data;

    printk("Public MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
           rp->bdaddr.val[5], rp->bdaddr.val[4], rp->bdaddr.val[3],
           rp->bdaddr.val[2], rp->bdaddr.val[1], rp->bdaddr.val[0]);

    net_buf_unref(rsp);
}

static void bt_ready(int err)
{
    if (err)
    {
        printk("Bluetooth init failed (err %d)\n", err);
        return;
    }
    printk("Bluetooth initialized\n");

    if (err)
    {
        printk("bluetooth advertise failed\n");
    }
}

uint8_t get_advertise_status(void)
{
    return ble_advertise_status;
}

static int create_advertising(void)
{
    int err;
#ifdef CONFIG_ENABLE_LE_CODED
    struct bt_le_adv_param param =
        BT_LE_ADV_PARAM_INIT(BT_LE_ADV_OPT_CONNECTABLE |
                                 BT_LE_ADV_OPT_EXT_ADV |
                                 BT_LE_ADV_OPT_USE_TX_POWER |
                                 BT_LE_ADV_OPT_CODED,
                             BT_GAP_ADV_FAST_INT_MIN_2,
                             BT_GAP_ADV_FAST_INT_MAX_2,
                             NULL);

    err = bt_le_ext_adv_create(&param, NULL, &adv);
    if (err)
    {
        printk("Failed to create advertiser set (err %d)\n", err);
        return err;
    }

    printk("Created adv: %p\n", adv);

    err = bt_le_ext_adv_set_data(adv, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err)
    {
        printk("Failed to set advertising data (err %d)\n", err);
        return err;
    }
#else
    // do nothing
#endif
    return 0;
}

int start_advertise(void)
{
    int err = 0;
    sensor_data[sizeof(sensor_data) - 1] = ADV_TX_POWER;
    sensor_data[sizeof(sensor_data) - 2] = adv_packet_no;
    if (adv_packet_no < MAX_ADVERTISE_PACKET)
    {
        ++adv_packet_no;
    }
    else
    {
        adv_packet_no = 1;
    }
#ifdef CONFIG_ENABLE_LE_CODED
    err = bt_le_ext_adv_start(adv, BT_LE_EXT_ADV_START_DEFAULT);
    if (err)
    {
        printk("Failed to start extended advertising set (err %d)\n", err);
        return;
    }
    // err = bt_le_adv_start(BT_LE_ADV_CONNECT_LE_CODED, sensor, ARRAY_SIZE(sensor), NULL, 0);
#else
    err = bt_le_adv_start(BT_LE_ADV_CONNECT, sensor, ARRAY_SIZE(sensor), NULL, 0);
#endif
    // err = bt_le_adv_start(BT_LE_ADV_NCONN /*BT_LE_ADV_CONN*/, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    if (err)
    {
        printk("Advertising failed to start (err %d)\n", err);
        return 0;
    }
    ble_advertise_status = BLE_ADVERTISING;
    // printk("advertising started\n");

    return err;
}

int stop_advertise(void)
{
    int err = 0;
#ifdef CONFIG_ENABLE_LE_CODED
    err = bt_le_ext_adv_stop(adv);
#else
    err = bt_le_adv_stop();
#endif
    if (err)
    {
        printk("Advertising failed to stop (err %d)\n", err);
    }
    ble_advertise_status = BLE_NOT_ADVERTISING;
    return err;
}

void bt_set_tx_power(uint8_t tx_power)
{
    int8_t txp_get = 0;
    printk("Get Tx power level ->");
    get_tx_power(BT_HCI_VS_LL_HANDLE_TYPE_ADV, 0, &txp_get);
    printk("-> default TXP = %d\n", txp_get);

    k_sleep(K_SECONDS(1));

    printk("Set Tx power level to %d\n", tx_power);
    set_tx_power(BT_HCI_VS_LL_HANDLE_TYPE_ADV, 0, (int8_t)tx_power);
    k_sleep(K_SECONDS(1));
    printk("Get Tx power level -> ");
    get_tx_power(BT_HCI_VS_LL_HANDLE_TYPE_ADV, 0, &txp_get);
    printk("TXP = %d\n", txp_get);
}
int radio_init(void)
{
    int err;

    printk("Initialize radio \n");
    err = bt_enable(bt_ready);
    if (err)
    {
        printk("Bluetooth init failed (err %d)\n", err);
        return 0;
    }

    // bt_set_tx_power(ADV_TX_POWER);

    create_advertising();
    printk("Bluetooth initialized\n");

    if (IS_ENABLED(CONFIG_SETTINGS))
    {
        settings_load();
    }
    // get_public_mac_address();
    return 0;
}
