#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_event.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/net_if.h>
#include "wifi.h"
#include "common.h"

// Defines
#define WIFI_MGMT_EVENTS (NET_EVENT_WIFI_SCAN_RESULT |		\
				NET_EVENT_WIFI_SCAN_DONE |		\
				NET_EVENT_WIFI_CONNECT_RESULT |		\
				NET_EVENT_WIFI_DISCONNECT_RESULT)

// To define
#define WIFI_SSID "<SSID>"
#define WIFI_PASS "<PWD>"

// Typedefs

// Variables
static struct k_sem wifi_connected;
static struct wifi_connect_req_params wifi_args;

// Functions
void Wifi_check_connect_result( struct net_if *iface, struct net_mgmt_event_callback* cb)
{
	const struct wifi_status *status = (const struct wifi_status *) cb->info;
	if (!status->status) {
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
			printf("\r\nWiFi disconnected.\r\n");
			LED_OFF();
			connect_WiFi();
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

	// Affichage du nom de l'interface réseau
	char iface_name[20];
	net_if_get_name(iface,iface_name,20);
	printf("Interface name : %s",iface_name);


	if( net_mgmt( NET_REQUEST_WIFI_CONNECT, iface, &wifi_args, sizeof(wifi_args) ) ) {
		perror("Failed to request connection to "WIFI_SSID);
	}
	k_sem_take(&wifi_connected, K_FOREVER);

	// Affichage de l'adresse IP de l'appareil sur le réseau
	struct net_if_ipv4 *ipv4 = iface->config.ip.ipv4;
	static char buf[NET_IPV4_ADDR_LEN];
	net_addr_ntop( AF_INET, (const char *)&ipv4->unicast[0].ipv4.address.in_addr, buf, NET_IPV4_ADDR_LEN);
	printk("\r\nSucessfully connected to "WIFI_SSID"\r\nAssigned IP address [%s] \r\n", buf);
}



