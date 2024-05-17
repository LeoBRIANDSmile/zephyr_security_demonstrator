#ifndef SRC_WIFI_H_
#define SRC_WIFI_H_

// Functions
void WiFi_Init(void);
void connect_WiFi(void);
void Wifi_check_connect_result( struct net_if *, struct net_mgmt_event_callback *);
void Wifi_event_listener( struct net_mgmt_event_callback *, uint32_t , struct net_if * );

#endif /* SRC_WIFI_H_ */