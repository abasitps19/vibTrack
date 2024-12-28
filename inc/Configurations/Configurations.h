#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#include "..\..\inc\nrfconfig.h"

#define ENABLE 1
#define DISABLE 0

#define CTX_PIN 17
#define CRX_PIN 19
#define CPS_PIN 6

#define STATUS_LED_BT832X 20
#define STATUS_LED_E73_833 29

#define STATUS_LED STATUS_LED_BT832X

#define GPIO_PORT DT_NODELABEL(gpio0) // Use GPIO0 (check your devicetree)

#define ADV_TX_POWER (int8_t)4
#define DEVICE_BEACON_TXPOWER_NUM 8

static const int8_t txpower[DEVICE_BEACON_TXPOWER_NUM] = {4, 0, -3, -8, -15, -18, -23, -30};

// #define CONFIG_BT_LBS_SECURITY_ENABLED
#undef CONFIG_BT_LBS_SECURITY_ENABLED

#endif //_APP_CONFIGURATION_H