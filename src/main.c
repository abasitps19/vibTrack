#include "../inc/vibSens.h"

int main(void)
{
	system_init();
	init_timers();

	while (1)
	{
		handle_advertise();
		// blink_led();
	}
}
