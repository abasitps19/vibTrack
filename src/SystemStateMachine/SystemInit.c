/*
  P0.17 -> ctx
  p0.19 -> crx
  p0.06 -> cps

*/

#include "../../inc/Configurations/Configurations.h"
#include "../../inc/SystemStateMachine/SystemInit.h"
#include "../../inc/BLE/ble.h"

#define RUN_STATUS_LED DK_LED1
#define STATUS_LED1 DK_LED2
#define STATUS_LED2 DK_LED3

#define USER_BUTTON DK_BTN1_MSK

#define RUN_LED_BLINK_INTERVAL 1000

// #define HRS_QUEUE_SIZE 16

#define ADVERTISE_DELAY_TIME 2
#define ADVERTISE_DURATION 200

#define STATUS1_BUTTON DK_BTN1_MSK
#define STATUS2_BUTTON DK_BTN2_MSK

// GPIO defination
// static bool app_button_state;
uint8_t radio_state = BLE_RADIO_STATE_DEFAULT;
uint8_t led_status = 0;

int board_init(void);

void cb_timer_app_sec(struct k_timer *timer_id);
void cb_timer_app_ms(struct k_timer *timer_id);
void cb_timer_app_ms_stop(struct k_timer *timer_id);

K_TIMER_DEFINE(timer_sec, cb_timer_app_sec, NULL);
K_TIMER_DEFINE(timer_ms, cb_timer_app_ms, cb_timer_app_ms_stop);

#ifdef CONFIG_ENABLE_BT_TX_POWER_STRONGEST
const struct device *gpio_dev;
int int_power_amplifire(uint8_t status)
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
#endif
/*
static void button_changed(uint32_t button_state, uint32_t has_changed)
{
    if (has_changed & USER_BUTTON)
    {
        uint32_t user_button_state = button_state & USER_BUTTON;
        printk("button state changed\n");
        // bt_lbs_send_button_state(user_button_state);
        app_button_state = user_button_state ? true : false;
        if (user_button_state == true)
        {
            printk("button pressed\n");
        }
        else
        {
            printk("button un pressed \n");
        }
    }
}
*/
/*
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
*/
int system_init(void)
{
    int err;
    err = board_init();
#ifdef CONFIG_ENABLE_BT_TX_POWER_STRONGEST
    int_power_amplifire(ENABLE);
#endif
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
    /*  err = init_button();
      if (err)
      {
          printk("Button init failed (err %d)\n", err);
      }
  */
    return err;
}

/*
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
*/

void cb_timer_app_sec(struct k_timer *timer_id)
{
    radio_state = BLE_RADIO_STATE_START_ADVERTISE;
    k_timer_start(&timer_ms, K_MSEC(ADVERTISE_DURATION), K_NO_WAIT);
}

void cb_timer_app_ms(struct k_timer *timer_id)
{
    radio_state = BLE_RADIO_STATE_STOP_ADVERTISE;
}
void cb_timer_app_ms_stop(struct k_timer *timer_id)
{
}

void init_timers(void)
{
    k_timer_start(&timer_sec, K_SECONDS(ADVERTISE_DELAY_TIME * 2), K_SECONDS(ADVERTISE_DELAY_TIME * 2));
}

void blink_led(void)
{
    // static int blink_status = 0;
    // dk_set_led(RUN_STATUS_LED, 0 /*(++blink_status) % 2*/);
    // dk_set_led(STATUS_LED1, 0);
    // dk_set_led(STATUS_LED2, 1);
    // k_sleep(K_MSEC(RUN_LED_BLINK_INTERVAL));
    // gpio_pin_set(gpio_dev, PIN_NUMBER, 1); // Set pin high
    // k_msleep(500);                         // Delay
    // gpio_pin_set(gpio_dev, PIN_NUMBER, 0); // Set pin low
    // k_msleep(500);                         // Delay
}

void process_radio_states(void)
{
    switch (radio_state)
    {
    case BLE_RADIO_STATE_DEFAULT:
        break;
    case BLE_RADIO_STATE_START_ADVERTISE:
        start_advertise();
        printk("start advertising \n");
        dk_set_led(RUN_STATUS_LED, 1);
        radio_state = BLE_RADIO_STATE_DEFAULT;
        break;
    case BLE_RADIO_STATE_START_ADVERTISE_CONTINUOUS:
        if (get_advertise_status() == BLE_NOT_ADVERTISING)
        {
            start_advertise();
            printk("advertising contious mode\n");
        }
        else
        {
            // printk("advertising already started\n");
        }
        break;
    case BLE_RADIO_STATE_STOP_ADVERTISE:
        stop_advertise();
        printk("stop advertising \n");
        dk_set_led(RUN_STATUS_LED, 0);
        radio_state = BLE_RADIO_STATE_DEFAULT;
        break;
    case BLE_RADIO_STATE_START_SCAN:
        break;
    case BLE_RADIO_STATE_STOP_SCAN:
        break;
    default:
        break;
    }
}
