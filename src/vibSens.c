/*
  P0.17 -> ctx
  p0.19 -> crx
  p0.06 -> cps

*/

#include "../inc/vibsens.h"

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <bluetooth/gatt_dm.h>
#include <bluetooth/scan.h>

#include <zephyr/bluetooth/services/bas.h>
#include <zephyr/bluetooth/services/hrs.h>
#include <bluetooth/services/hrs_client.h>

#include <dk_buttons_and_leds.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#include <zephyr/settings/settings.h>

#include <zephyr/kernel.h>

// #include <sys/byteorder.h>
#include <stdio.h>
// #include <bluetooth/hci_vs.h>

// #include <nrfx_gpio.h>

#define STACKSIZE 1024
#define PRIORITY 7

#define RUN_STATUS_LED DK_LED1
#define STATUS_LED1 DK_LED2
#define STATUS_LED2 DK_LED3

#define RUN_LED_BLINK_INTERVAL 1000

// #define HRS_QUEUE_SIZE 16

#define ADVERTISE_DELAY_TIME 2
#define ADVERTISE_DURATION 1000
#define MAX_ADVERTISE_PACKET 250

#define STATUS1_BUTTON DK_BTN1_MSK
#define STATUS2_BUTTON DK_BTN2_MSK

// GPIO defination
#define ENABLE 1
#define DISABLE 0

#define CTX_PIN 17
#define CRX_PIN 19
#define CPS_PIN 6

#define STATUS_LED_BT832X 20
#define STATUS_LED_E73_833 29

#define STATUS_LED STATUS_LED_BT832X
#define GPIO_PORT DT_NODELABEL(gpio0) // Use GPIO0 (check your devicetree)

int radio_init(void);
int board_init(void);

void cb_timer_app_1sec(struct k_timer *timer_id);
void cb_timer_app_100ms(struct k_timer *timer_id);
void cb_timer_app_100ms_stop(struct k_timer *timer_id);

K_TIMER_DEFINE(timer_1sec, cb_timer_app_1sec, NULL);
K_TIMER_DEFINE(timer_100ms, cb_timer_app_100ms, cb_timer_app_100ms_stop);

uint8_t led_status = 0;
uint8_t adv_packet_no = 0;

const struct device *gpio_dev;

#define MAX_TX_POWER_NRF52832 0
#define MAX_TX_POWER_NRF52833 0

// static uint8_t txPower = 8;

uint8_t sensor_data[] = {
    0x4C, 0x00, // 0x59 00 nordic, 0x4c 00 apple
    0x02, 0x15,
    0x1A, 0xCC, 0x1B, 0x16, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
    0x00, 0x00, 0x00, 0x00, 0x00};

static const struct bt_data sensor[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR | BT_LE_AD_GENERAL),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, sensor_data, sizeof(sensor_data)),
};
/*
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};
static const struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_NSMS_VAL),
};
*/

/*void set_tx_power(int8_t tx_power)
{
    NRF_RADIO->TXPOWER = (tx_power << RADIO_TXPOWER_TXPOWER_Pos) & RADIO_TXPOWER_TXPOWER_Msk;
}
*/

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
}

static int int_power_amplifire(uint8_t status)
{
    int err = 0;

    gpio_dev = DEVICE_DT_GET(GPIO_PORT);
    if (!device_is_ready(gpio_dev))
    {
        printk("GPIO device not ready\n");
        return 1;
    }

    gpio_pin_configure(gpio_dev, CTX_PIN, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure(gpio_dev, CRX_PIN, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure(gpio_dev, CPS_PIN, GPIO_OUTPUT_ACTIVE);

    if (status == ENABLE)
    {
        // set sky66112 power mode transmit high power
        gpio_pin_set(gpio_dev, CPS_PIN, 0); // CPS = 0  CPS_PIN = 6
        gpio_pin_set(gpio_dev, CTX_PIN, 1); // CTX = 1  CTX_PIN = 17
        gpio_pin_set(gpio_dev, CRX_PIN, 0); // CRX = X  CRX_PIN = 19
    }
    else if (status == DISABLE)
    {
        // set sky66112 power mode transmit low power
        gpio_pin_set(gpio_dev, CPS_PIN, 0); // CPS = 1  CPS_PIN = 6
        gpio_pin_set(gpio_dev, CTX_PIN, 0); // CTX = 0  CTX_PIN = 17
        gpio_pin_set(gpio_dev, CRX_PIN, 0); // CRX = X  CRX_PIN = 19
    }

    return err;
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
    err = board_init();
    err = radio_init();
    // int_power_amplifire(ENABLE);
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

    /*  err = init_button();
      if (err)
      {
          printk("Button init failed (err %d)\n", err);
          return 0;
      }
      */
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
    // bt_ctlr_set_public_addr();
    // bt_ctlr_set_static_addr();

    printk("Bluetooth initialized\n");

    if (IS_ENABLED(CONFIG_SETTINGS))
    {
        settings_load();
    }
    // get_public_mac_address();
    return 0;
}

int start_advertise(void)
{
    int err = 0;
    uint8_t tx_power = 0;
    // set_tx_power(4);
    tx_power = get_tx_power();
    set_tx_power(BT_HCI_VS_LL_HANDLE_TYPE_ADV, 0, 8);
    sensor_data[sizeof(sensor_data) - 1] = tx_power;
    sensor_data[sizeof(sensor_data) - 2] = adv_packet_no;
    if (adv_packet_no < MAX_ADVERTISE_PACKET)
    {
        ++adv_packet_no;
    }
    else
    {
        adv_packet_no = 1;
    }

    err = bt_le_adv_start(BT_LE_ADV_CONN, sensor, ARRAY_SIZE(sensor), NULL, 0);

    // err = bt_le_adv_start(BT_LE_ADV_NCONN /*BT_LE_ADV_CONN*/, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));

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
        dk_set_led(RUN_STATUS_LED, 0);
        advertise_state = 3;
    }
    else if (advertise_state == 2)
    {
        stop_advertise();
        printk("stop advertising \n");
        dk_set_led(RUN_STATUS_LED, 1);
        advertise_state = 3;
    }
    else
    {
    }
};

void cb_timer_app_1sec(struct k_timer *timer_id)
{

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
    // static int blink_status = 0;
    // dk_set_led(RUN_STATUS_LED, 0 /*(++blink_status) % 2*/);
    // dk_set_led(STATUS_LED1, 0);
    // dk_set_led(STATUS_LED2, 1);
    // k_sleep(K_MSEC(RUN_LED_BLINK_INTERVAL));
    //   gpio_pin_set(gpio_dev, PIN_NUMBER, 1); // Set pin high
    //    k_msleep(500);                         // Delay
    //    gpio_pin_set(gpio_dev, PIN_NUMBER, 0); // Set pin low
    //    k_msleep(500);                         // Delay
}
