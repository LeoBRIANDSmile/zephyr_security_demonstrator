#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "common.h"

#define STACK_SIZE 1024
#define PRIORITY 7

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void blink_led() {
    static int led_state = 0;

    while (1) {
	    gpio_pin_set_dt(&led,led_state);
        k_msleep(500);
        led_state = !led_state;
    }
}

K_THREAD_DEFINE(blink_led_id, STACK_SIZE, blink_led, NULL, NULL, NULL, PRIORITY, 0, 0);




