// Includes
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <string.h>
#include "common.h"


#define MAX_SIZE_BUFFER_RECEIVE 10000

// States
#define WIFI_CONNECTION_REQUEST         0
#define SOCKET_CREATION_STATE           1
#define SEND_STATE                      2
#define RECEIVE_STATE                   3
#define IDLE_STATE                      -1

// Local variables
char dataToSend[100];
char dataToRecv[MAX_SIZE_BUFFER_RECEIVE];

// Extern variables
int state = WIFI_CONNECTION_REQUEST;

// Main
int main(void){
	// Initialisations
	WiFi_Init();
	strcpy(dataToSend,"Received");


	while (1) {
		// Machine d'état
		switch(state){
			case(WIFI_CONNECTION_REQUEST):
				connect_WiFi();
				state = SOCKET_CREATION_STATE;
				break;
			case(SOCKET_CREATION_STATE):
				Socket_Init();
				state = SEND_STATE;
				break;
			case(SEND_STATE):
				Socket_Send(dataToSend);
				state = RECEIVE_STATE;
			case(RECEIVE_STATE):
				Socket_Receive(dataToRecv);
				state = IDLE_STATE;
			case(IDLE_STATE):
				break;
		}
	}
}