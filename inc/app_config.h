#ifndef _APP_CONFIG_H
#define _APP_CONFIG_H

#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/byteorder.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/hci_vs.h>

#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/services/hrs.h>
#include <zephyr/device.h>
#include <zephyr/settings/settings.h>
#include <zephyr/drivers/gpio.h>

#include <dk_buttons_and_leds.h>


#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/byteorder.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/hci_vs.h>

#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/services/hrs.h>


#define ENABLE 1
#define DISABLE 0

#define CTX_PIN 17
#define CRX_PIN 19
#define CPS_PIN 6

#define STATUS_LED_BT832X 20
#define STATUS_LED_E73_833 29

#define STATUS_LED STATUS_LED_BT832X

#define GPIO_PORT DT_NODELABEL(gpio0) // Use GPIO0 (check your devicetree)

#define ADV_TX_POWER (int8_t) 4
#define DEVICE_BEACON_TXPOWER_NUM  8
static const int8_t txpower[DEVICE_BEACON_TXPOWER_NUM] = {4, 0, -3, -8,
							  -15, -18, -23, -30};

#endif //_APP_CONFIG_H