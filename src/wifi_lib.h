#ifndef SRC_WIFI_LIB_H_
#define SRC_WIFI_LIB_H_

// States
#define WIFI_CONNECTION_REQUEST 		0
#define WIFI_CONNECTED 					1
#define SOCKET_SEND_REQUEST 			2
#define SOCKET_RECEIVE_REQUEST 			3
#define IDLE_STATE                      4

// Functions
void WiFi_Init(void);
void connect_WiFi(void);
void Wifi_check_connect_result( struct net_if *, struct net_mgmt_event_callback *);
void Wifi_event_listener( struct net_mgmt_event_callback *, uint32_t , struct net_if * );
void Socket_Init(void);
void Socket_Send(char*);
void Socket_Receive(char*);
void LED_Init(void);
void LED_ON(void);
void LED_OFF(void);

#endif /* SRC_WIFI_LIB_H_ */