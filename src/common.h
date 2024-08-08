#ifndef __COMMON_H__
#define __COMMON_H__

#define MAX_SIZE_BUFFER         2048
#define CERT_SIZE               779
#define FLASH_SECTOR_SIZE		4096
#define SLOT1_PARTITION_SIZE	FIXED_PARTITION_SIZE(slot1_partition)

// Global variables
static char dataToRecv[MAX_SIZE_BUFFER];

#define HTTP_HOST "https://172.20.10.3" 
#define HTTP_PORT 443
#define REQUEST /*"GET " HTTP_PATH*/ " HTTP/1.0\r\nHost: " HTTP_HOST "\r\n\r\n"

int Socket_Init(void);
int Socket_Send(char *);
int Socket_Close(void);
int Socket_Receive(char*);
int Socket_Receive_to_tab(char*);
int Socket_Receive_firmware_to_flash(void);

void WiFi_Init(void);
int connect_WiFi(char*,char*);

void LED_ON(void);
void LED_OFF(void);
void LED_Init(void);

int flash_load_first_cert(char*);
int flash_load_new_cert(char*);
char* flash_read_cert(int);
int swap_certs(void);

int http_get_new_cert(char*);

void progress_bar(int, int);

#endif
