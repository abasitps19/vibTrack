/*
  P0.17 -> ctx
  p0.19 -> crx
  p0.06 -> cps

*/
#include "../inc/app_config.h"
#include "../inc/ble.h"
#include "../inc/vibsens.h"

/*
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


#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#include <zephyr/settings/settings.h>

#include <zephyr/kernel.h>

// #include <sys/byteorder.h>
#include <stdio.h>
// #include <bluetooth/hci_vs.h>

// #include <nrfx_gpio.h>
*/
//#define STACKSIZE 1024
//#define PRIORITY 7

#define RUN_STATUS_LED DK_LED1
#define STATUS_LED1 DK_LED2
#define STATUS_LED2 DK_LED3

#define RUN_LED_BLINK_INTERVAL 1000

// #define HRS_QUEUE_SIZE 16

#define ADVERTISE_DELAY_TIME 2
#define ADVERTISE_DURATION 1000


#define STATUS1_BUTTON DK_BTN1_MSK
#define STATUS2_BUTTON DK_BTN2_MSK

// GPIO defination


int board_init(void);

void cb_timer_app_1sec(struct k_timer *timer_id);
void cb_timer_app_100ms(struct k_timer *timer_id);
void cb_timer_app_100ms_stop(struct k_timer *timer_id);

K_TIMER_DEFINE(timer_1sec, cb_timer_app_1sec, NULL);
K_TIMER_DEFINE(timer_100ms, cb_timer_app_100ms, cb_timer_app_100ms_stop);

uint8_t led_status = 0;



static int init_button(void)
{
    int err;
    //err = dk_buttons_init(button_changed);
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

static uint8_t advertise_state = 0;

void handle_advertise()
{
    if (advertise_state == 1)
    {
        //start_advertise();
        //printk("start advertising \n");
        dk_set_led(RUN_STATUS_LED, 0);
        advertise_state = 3;
    }
    else if (advertise_state == 2)
    {
        //stop_advertise();
        //printk("stop advertising \n");
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
