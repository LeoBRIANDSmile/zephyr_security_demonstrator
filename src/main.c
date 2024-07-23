// Includes
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <string.h>
#include "common.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);


// States
#define WIFI_CONNECTION_REQUEST         0
#define SOCKET_CREATION_STATE           1
#define SEND_STATE                      2
#define RECEIVE_STATE                   3
#define IDLE_STATE                      -1

// Local variables
char dataToSend[100];
char dataToRecv[MAX_SIZE_BUFFER];

// Extern variables
int state = WIFI_CONNECTION_REQUEST;

// Main
int main(void){
	// Initialisations
	WiFi_Init();
	strcpy(dataToSend,"Succesfully connected");
	int ret = 0;

	while (1) {
		// Machine d'état
		switch(state){
			case(WIFI_CONNECTION_REQUEST):
				LOG_INF("State : WIFI_CONNECTION_REQUEST");
				ret = connect_WiFi();
				if(ret){
					state = SOCKET_CREATION_STATE;
				}
				break;
			case(SOCKET_CREATION_STATE):
				LOG_INF("State : SOCKET_CREATION_STATE");
				ret = Socket_Init();
				if(ret){
					state = SEND_STATE;				
				}
				break;
			case(SEND_STATE):
				LOG_INF("State : SEND_STATE");
				ret = Socket_Send(REQUEST);
				if(ret){
					state = RECEIVE_STATE;			
				}
			case(RECEIVE_STATE):
				LOG_INF("State : RECEIVE_STATE");
				ret = Socket_Receive(dataToRecv);
				if(ret){
					state = IDLE_STATE;		
				}
			case(IDLE_STATE):
				// LOG_INF("State : IDLE_STATE");
				ret = !ret;
				break;
		}
	}
}