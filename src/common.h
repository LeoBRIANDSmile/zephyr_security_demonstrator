#define MAX_SIZE_BUFFER 1024

#define HTTP_HOST "https://172.20.10.2" 
#define HTTP_PORT 443
#define HTTP_PATH "/certs/CA.pem"
#define REQUEST "GET " HTTP_PATH " HTTP/1.0\r\nHost: " HTTP_HOST "\r\n\r\n"

void Socket_Init(void);
void Socket_Send(char * data);
int Socket_Receive(char* data);
void WiFi_Init(void);
void connect_WiFi(void);
void LED_ON(void);
void LED_OFF(void);
void LED_Init(void);