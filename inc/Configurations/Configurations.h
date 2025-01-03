#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#include "..\..\inc\nrfconfig.h"

#define ADV_TX_POWER (int8_t)4 // dBm

//#define CONFIG_ENABLE_BT_TX_POWER_STRONGEST 1
//  #undef CONFIG_ENABLE_BT_TX_POWER_STRONGEST

#define ENABLE 1
#define DISABLE 0
#ifdef CONFIG_ENABLE_BT_TX_POWER_STRONGEST
#define CTX_PIN 17
#define CRX_PIN 19
#define CPS_PIN 6
#define GPIO_PORT DT_NODELABEL(gpio0) // Use GPIO0 (check your devicetree)
#endif

#define STATUS_LED_BT832X 20
#define STATUS_LED_E73_833 29

#define STATUS_LED STATUS_LED_BT832X

#endif //_APP_CONFIGURATION_H