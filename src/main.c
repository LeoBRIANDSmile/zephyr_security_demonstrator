// Includes
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <string.h>
#include "wifi.h"

// States
#define WIFI_CONNECTION_REQUEST			0
#define IDLE_STATE						-1

// Local variables

// Extern variables
int state = WIFI_CONNECTION_REQUEST;

// Main
int main(void){
	// Initialisations
	WiFi_Init();

	while (1) {
		// Machine d'état
		switch(state){
			case(WIFI_CONNECTION_REQUEST):
				connect_WiFi();
				state = IDLE_STATE;
				break;
			case(IDLE_STATE):
				printf("\r-----------------Idle State-----------------");
				break;
		}
	}
}