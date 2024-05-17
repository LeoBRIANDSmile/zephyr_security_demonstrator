#include <zephyr/drivers/gpio.h>
#include "led.h"


#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void LED_ON(void){
	gpio_pin_set_dt(&led,1);
}

void LED_OFF(void){
	gpio_pin_set_dt(&led,0);
}

void LED_Init(void){
	// Initialisation LED
	gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	LED_OFF();
}