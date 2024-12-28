#ifndef _SYSTEM_INIT_H
#define _SYSTEM_INIT_H

void blink_led();
void init_timers(void);
int system_init(void);
void handle_advertise();
int start_advertise(void);
int stop_advertise(void);

#endif //_SYSTEM_INIT_H