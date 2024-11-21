#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <soc.h>

#include "..\inc\vibSens.h"

int main(void)
{
	system_init();
	start_advertise();
	while(1)
	{
		blink_led();
	}
	
}
