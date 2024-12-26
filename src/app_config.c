#include "../inc/app_config.h"

const struct device *gpio_dev;
static int int_power_amplifire(uint8_t status)
{
    int err = 0;

    gpio_dev = DEVICE_DT_GET(GPIO_PORT);
    if (!device_is_ready(gpio_dev))
    {
        printk("GPIO device not ready\n");
        return 1;
    }

    gpio_pin_configure(gpio_dev, CTX_PIN, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure(gpio_dev, CRX_PIN, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure(gpio_dev, CPS_PIN, GPIO_OUTPUT_ACTIVE);

    if (status == ENABLE)
    {
        // set sky66112 power mode transmit high power
        gpio_pin_set(gpio_dev, CPS_PIN, 0); // CPS = 0  CPS_PIN = 6
        gpio_pin_set(gpio_dev, CTX_PIN, 1); // CTX = 1  CTX_PIN = 17
        gpio_pin_set(gpio_dev, CRX_PIN, 0); // CRX = X  CRX_PIN = 19
    }
    else if (status == DISABLE)
    {
        // set sky66112 power mode transmit low power
        gpio_pin_set(gpio_dev, CPS_PIN, 0); // CPS = 1  CPS_PIN = 6
        gpio_pin_set(gpio_dev, CTX_PIN, 0); // CTX = 0  CTX_PIN = 17
        gpio_pin_set(gpio_dev, CRX_PIN, 0); // CRX = X  CRX_PIN = 19
    }

    return err;
}