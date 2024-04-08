#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_event.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/net_if.h>
#include <zephyr/drivers/gpio.h>

#include "wifi_lib.h"


#define WIFI_SSID "iPhonedeLeo"
#define WIFI_PASS "leobriiand"

// #define WIFI_SSID "Redmi"
// #define WIFI_PASS "12345678"

// #define WIFI_SSID "SmileWiFi"
// #define WIFI_PASS "SmileWiFi"


#define WIFI_MGMT_EVENTS (NET_EVENT_WIFI_SCAN_RESULT |		\
				NET_EVENT_WIFI_SCAN_DONE |		\
				NET_EVENT_WIFI_CONNECT_RESULT |		\
				NET_EVENT_WIFI_DISCONNECT_RESULT)
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define IP_ADDRESS "172.20.10.2"
#define PORT 80
#define LED0_NODE DT_ALIAS(led0)


// Typedefs
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;



int state = WIFI_CONNECTION_REQUEST;
static struct net_mgmt_event_callback* cb;
static struct k_sem wifi_connected;
static struct wifi_connect_req_params wifi_args;
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
SOCKET sock;

void Wifi_check_connect_result( struct net_if *iface, struct net_mgmt_event_callback *cb)
{
	const struct wifi_status *status = (const struct wifi_status *) cb->info;
	if (!status->status) {
		// Connected
		//printf("\r\nConnected to "WIFI_SSID".\r\n");
		LED_ON();
		k_sem_give(&wifi_connected);
	}
}

void Wifi_event_listener( struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface ) {
	switch( mgmt_event ) {
		case NET_EVENT_WIFI_CONNECT_RESULT:
			Wifi_check_connect_result( iface, cb );
			break;
		case NET_EVENT_WIFI_DISCONNECT_RESULT:
			state = WIFI_CONNECTION_REQUEST;
			printf("\r\nWiFi disconnected.\r\n");
			LED_OFF();
			break;
	}
}


void WiFi_Init(void){
	LED_Init();
	printf("\r\nWiFi initialisation...\r\n");
    k_sem_init(&wifi_connected, 0, 1);

	// Configuration Callback
	net_mgmt_init_event_callback(&cb, Wifi_event_listener, WIFI_MGMT_EVENTS );
	net_mgmt_add_event_callback(&cb);

	// Configuration des paramètres Wi-Fi
	wifi_args.security = WIFI_SECURITY_TYPE_PSK;
	wifi_args.channel = WIFI_CHANNEL_ANY;
	wifi_args.psk = WIFI_PASS;
	wifi_args.psk_length = strlen(WIFI_PASS);
	wifi_args.ssid = WIFI_SSID;
	wifi_args.ssid_length = strlen(WIFI_SSID);
	printf("\r\nWiFi initialisation done.\r\n");
}

void connect_WiFi(void){
	printf("\r\nConnecting to WiFi...\r\n");
    // Requête de connexion au réseau
	struct net_if *iface = net_if_get_default();
	
	if( net_mgmt( NET_REQUEST_WIFI_CONNECT, iface, &wifi_args, sizeof(wifi_args) ) ) {
		perror("Failed to request connection to "WIFI_SSID);
	}
    k_sem_take(&wifi_connected, K_FOREVER);

	// Affichage de l'adresse IP de l'appareil sur le réseau
	struct net_if_ipv4 *ipv4 = iface->config.ip.ipv4;
	static char buf[NET_IPV4_ADDR_LEN];
	net_addr_ntop( AF_INET, (const char *)&ipv4->unicast[0].ipv4.address.in_addr, buf, NET_IPV4_ADDR_LEN);
	printk("\r\nSucessfully connected to "WIFI_SSID"\r\nAssigned IP address [%s] \r\n", buf);
	state = WIFI_CONNECTED;
	
}

void Socket_Init(void){
	//Création du socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET)
	{
		printf("\r\nSocket creation error (socket())\r\n");
	}
	else{
		printf("\r\nCreated socket : %d\r\n", sock);
	}

	
	SOCKADDR_IN sin = { 0 }; /* initialise la structure avec des 0 */
	// if(inet_pton(AF_INET,IP_ADDRESS,&sin.sin_addr)!=1){ // A vérfier avec net_addr_pton
	// 	printf("\r\nIP Address issue\r\n");
	// }
	net_addr_pton(AF_INET,IP_ADDRESS,&sin.sin_addr);
	//inet_pton(AF_INET,IP_ADDRESS,&sin.sin_addr);
	//sin.sin_addr.s_addr = inet_addr(IP_ADDRESS) ; 
	sin.sin_port = htons(PORT); /* on utilise htons pour le port */
	sin.sin_family = AF_INET;
	

	printf("\r\nWaiting for connection on port %d at IP %s (IPv4)...\r\n",PORT,IP_ADDRESS);
	int ret;
	// Connexion au serveur
	ret = connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR));

	while(ret!=0){
		printf("\r\nSocket connection error : %d(connect())\r\n",ret);
		printf("\r\nRetrying to connect\r\n",PORT,IP_ADDRESS);
		ret = connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR));
		k_sleep(K_MSEC(5000));
	}
	printf("\r\nConnection succeeded at [%s] (connect())\r\n",IP_ADDRESS);
	state = SOCKET_SEND_REQUEST;



}

void Socket_Send(char * data){
	if(send(sock, data, strlen(data), 0) < 0)
	{
		printf("\r\nSend error (send())\r\n");
	}
	else{
		printf("\r\nSend succeeded, message : '%s' (send())\r\n",data);
		state = SOCKET_RECEIVE_REQUEST;
	}
}

void Socket_Receive(char* data){
	// to do
	int size = 0;
	//while(recv(sock, data, strlen(data), 0)<=0);
	if(recv(sock, data, strlen(data), 0)<0 )
	{
		printf("\r\nReceive error (recv())\r\n");
		state = IDLE_STATE;
	}
	else{
		// printf("\r\nReceive succeeded, message : '%s' (recv())\r\n",data);
		printf("%s",data);
		state = IDLE_STATE;
	}
}


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