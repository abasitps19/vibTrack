
#ifndef _NRF_CONFIG_H
#define _NRF_CONFIG_H

#include <stddef.h>
#include <errno.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/types.h>

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
#include <zephyr/bluetooth/services/bas.h>
#include <bluetooth/services/lbs.h>

#include <zephyr/device.h>
#include <zephyr/settings/settings.h>
#include <zephyr/drivers/gpio.h>

#include <dk_buttons_and_leds.h>


#endif //_NRF_CONFIG_H