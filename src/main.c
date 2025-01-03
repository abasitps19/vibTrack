// #include "../inc/vibSens.h"

#include "../inc/SystemStateMachine/SystemInit.h"
int main(void)
{
	system_init();
	init_timers();
	// blink_led();

	while (1)
	{
		process_radio_states();
		// blink_led();
	}
}
