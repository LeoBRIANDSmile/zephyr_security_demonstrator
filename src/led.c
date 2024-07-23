#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(led, LOG_LEVEL_DBG);

#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void LED_ON(void){
	gpio_pin_set_dt(&led,1);
	LOG_INF("LED ON");
}

void LED_OFF(void){
	gpio_pin_set_dt(&led,0);
	LOG_INF("LED OFF");

}

void LED_Init(void){
	// Initialisation LED
	gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	LED_OFF();
	LOG_INF("LED initialized");

}