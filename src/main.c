// Includes
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <string.h>
#include "common.h"
#include <zephyr/logging/log.h>
#include "ca_certificate.h"
#include <zephyr/dfu/mcuboot.h>

// Main
int main(void){
	if (boot_write_img_confirmed()<0) {
		printf("\r\nCan't confirm image\r\n");
	}
	// First firmware flash then comment
	flash_load_first_cert(ca_certificate);

	WiFi_Init();
	return 0;
}