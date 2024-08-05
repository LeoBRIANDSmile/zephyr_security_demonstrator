// Includes
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <string.h>
#include "common.h"
#include <zephyr/logging/log.h>
#include "ca_certificate.h"

// Main
int main(void){
	flash_load_first_cert(ca_certificate);
	// flash_load_new_cert(ca_certificate);
	WiFi_Init();
	return 0;
}