// Includes
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include "wifi_lib.h"

// Local variables
char dataToSend[MAX_SIZE_BUFFER];
char dataToRecv[MAX_SIZE_BUFFER];
int tmp = 0;

// Extern variables
extern int state;

// Main
int main(void){
	// Initialisations
	WiFi_Init();

	// Connexion automatique au réseau
	connect_WiFi();
	Socket_Init();

	strcpy(dataToSend,"Hello World !");

    while (1) {

		// Machine d'état
		switch(state){
			case(WIFI_CONNECTION_REQUEST):
				connect_WiFi();
				Socket_Init();
				break;
			case(SOCKET_SEND_REQUEST):
				Socket_Send(dataToSend);
				break;
			case(SOCKET_RECEIVE_REQUEST):
				Socket_Receive(&dataToRecv);
				break;
			case(SOCKET_CLOSE_REQUEST):
				Socket_Close();
				break;
			case(IDLE_STATE):
				break;
			default:
				Socket_Close();

		}
    }
}