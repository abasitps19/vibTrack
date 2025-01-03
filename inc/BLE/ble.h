#ifndef _BLE_H
#define _BLE_H

extern int radio_init(void);
extern uint8_t get_advertise_status(void);
extern int stop_advertise(void);
extern int start_advertise(void);

#define BLE_ADVERTISING 0x01
#define BLE_NOT_ADVERTISING 0x02

#define BLE_RADIO_STATE_DEFAULT 0x00
#define BLE_RADIO_STATE_START_ADVERTISE 0x01
#define BLE_RADIO_STATE_START_ADVERTISE_CONTINUOUS 0x02
#define BLE_RADIO_STATE_STOP_ADVERTISE 0x03
#define BLE_RADIO_STATE_START_SCAN 0x04
#define BLE_RADIO_STATE_STOP_SCAN 0x05

/*
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};
static const struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_NSMS_VAL),
};
*/

#endif //_BLE_H