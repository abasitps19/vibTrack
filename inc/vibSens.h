#ifndef _VIB_SENSOR_H
#define _VIB_SENSOR_H

void blink_led();
void init_timers(void);
int system_init(void);
void handle_advertise();
int start_advertise(void);
int stop_advertise(void);

#endif //_VIB_SENSOR_H