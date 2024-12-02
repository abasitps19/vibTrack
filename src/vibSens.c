
#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <soc.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include <bluetooth/services/nsms.h>

#include <zephyr/settings/settings.h>

#include <dk_buttons_and_leds.h>

#include "..\inc\vibSens.h"

#define DEVICE_NAME "Sensor Status" // CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

#define RUN_STATUS_LED DK_LED1
#define CON_STATUS_LED DK_LED2
#define RUN_LED_BLINK_INTERVAL 1000

#define ADVERTISE_DELAY_TIME 2
#define ADVERTISE_DURATION 200

#define STATUS1_BUTTON DK_BTN1_MSK
#define STATUS2_BUTTON DK_BTN2_MSK

/* Implementation of two status characteristics */
BT_NSMS_DEF(nsms_btn1, "Button 1", false, "Unknown", 20);
BT_NSMS_DEF(nsms_btn2, "Button 2", IS_ENABLED(CONFIG_BT_STATUS_SECURITY_ENABLED), "Unknown", 20);

int radio_init(void);
int board_init(void);

void cb_timer_app_1sec(struct k_timer *timer_id);
void cb_timer_app_100ms(struct k_timer *timer_id);
void cb_timer_app_100ms_stop(struct k_timer *timer_id);

K_TIMER_DEFINE(timer_1sec, cb_timer_app_1sec, NULL);
K_TIMER_DEFINE(timer_100ms, cb_timer_app_100ms, cb_timer_app_100ms_stop);

uint8_t led_status = 0;

static uint8_t txPower = 4;

uint8_t sensor_data[] = {
    0x59, 0x00,
    0x08, 0x14,
    0x18, 0xee, 0x15, 0x16,             // UUID[15..12]
    0x01, 0x6b,                         // UUID[11..10]
    0x4b, 0xec,                         // UUID[9..8]
    0xad, 0x96,                         // UUID[7..6]
    0xbc, 0xb9, 0x6d, 0x16, 0x6e, 0x97, // UUID[5..0]
    0x00, 0x05,                         // Major
    0x00, 0x0a,                         // Minor
    0x02                                // IBEACON_RSSI (placeholder)
};

static const struct bt_data sensor[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, sensor_data, sizeof(sensor_data)),
};


static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};
static const struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_NSMS_VAL),
};

void set_tx_power(int8_t tx_power)
{
    NRF_RADIO->TXPOWER = (tx_power << RADIO_TXPOWER_TXPOWER_Pos) & RADIO_TXPOWER_TXPOWER_Msk;
}

int8_t get_tx_power(void)
{
    return (NRF_RADIO->TXPOWER & RADIO_TXPOWER_TXPOWER_Msk) >> RADIO_TXPOWER_TXPOWER_Pos;
}
void get_public_mac_address(void)
{
    struct net_buf *rsp;
    struct bt_hci_rp_read_bd_addr *rp;
    int err;
    
    // Send HCI command to read the BD_ADDR (public address)
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

static void button_changed(uint32_t button_state, uint32_t has_changed)
{
    if (has_changed & STATUS1_BUTTON)
    {
        bt_nsms_set_status(&nsms_btn1,
                           (button_state & STATUS1_BUTTON) ? "Pressed" : "Released");
    }
    if (has_changed & STATUS2_BUTTON)
    {
        bt_nsms_set_status(&nsms_btn2,
                           (button_state & STATUS2_BUTTON) ? "Pressed" : "Released");
    }
}

static int init_button(void)
{
    int err;
    err = dk_buttons_init(button_changed);
    if (err)
    {
        printk("Cannot init buttons (err: %d)\n", err);
    }
    return err;
}

int system_init(void)
{
    int err;
    err= board_init();
    err = radio_init();
    return err;
}

int board_init(void)
{
    int err;
    printk("sensor init\n");
    err = dk_leds_init();
    if (err)
    {
        printk("LEDs init failed (err %d)\n", err);
        return 0;
    }

    err = init_button();
    if (err)
    {
        printk("Button init failed (err %d)\n", err);
        return 0;
    }
    return err;
}
int radio_init(void)
{
    int err;
    err = bt_enable(NULL);
    if (err)
    {
        printk("Bluetooth init failed (err %d)\n", err);
        return 0;
    }
    bt_ctlr_set_public_addr();

    //bt_ctlr_set_static_addr();

    printk("Bluetooth initialized\n");

    if (IS_ENABLED(CONFIG_SETTINGS))
    {
        settings_load();
    }
    get_public_mac_address();

}

int start_advertise(void)
{
    int err = 0;
    uint8_t tx_power = 0;
    set_tx_power(txPower);
    tx_power = get_tx_power();
    // sensor_data[sizeof(sensor_data) - 1] = (uint8_t)tx_power;

    //err = bt_le_adv_start(BT_LE_ADV_NCONN, sensor, ARRAY_SIZE(sensor), NULL, 0);
     err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    // auth_passkey_display();
    if (err)
    {
        printk("Advertising failed to start (err %d)\n", err);
        return 0;
    }
    return err;
}

int stop_advertise(void)
{
    int err = bt_le_adv_stop();
    return err;
}
static uint8_t advertise_state = 0;

void handle_advertise()
{
    if (advertise_state == 1)
    {
        start_advertise();
        printk("start advertising \n");
        dk_set_led(RUN_STATUS_LED, 1);
        advertise_state = 3;
    }
    else if (advertise_state == 2)
    {
        stop_advertise();
        printk("stop advertising \n");
        dk_set_led(RUN_STATUS_LED, 0);
        advertise_state = 3;
    }
    else
    {
    }
};

void cb_timer_app_1sec(struct k_timer *timer_id)
{
    // turn on led
    // dk_set_led(RUN_STATUS_LED, 1);
    advertise_state = 1;
    k_timer_start(&timer_100ms, K_MSEC(ADVERTISE_DURATION), K_NO_WAIT);
    // start advertise
}

void cb_timer_app_100ms(struct k_timer *timer_id)
{
    // turn off led
    // stop_advertise();
    advertise_state = 2;
    // dk_set_led(RUN_STATUS_LED, 0);
}
void cb_timer_app_100ms_stop(struct k_timer *timer_id)
{
}

void init_timers(void)
{
    k_timer_start(&timer_1sec, K_SECONDS(ADVERTISE_DELAY_TIME), K_SECONDS(ADVERTISE_DELAY_TIME));
}

void blink_led(void)
{
    static int blink_status = 0;
    dk_set_led(RUN_STATUS_LED, (++blink_status) % 2);
    // k_sleep(K_MSEC(RUN_LED_BLINK_INTERVAL));
}
