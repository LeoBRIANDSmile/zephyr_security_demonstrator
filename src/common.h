#define MAX_SIZE_BUFFER 2048

// Global variables
static char dataToRecv[MAX_SIZE_BUFFER];

#define HTTP_HOST "https://172.20.10.3" 
#define HTTP_PORT 443
#define HTTP_PATH "/certs/CA.pem"
#define REQUEST /*"GET " HTTP_PATH*/ " HTTP/1.0\r\nHost: " HTTP_HOST "\r\n\r\n"

int Socket_Init(void);
int Socket_Send(char *);
int Socket_Close(void);
int Socket_Receive(char*);

void WiFi_Init(void);
int connect_WiFi(char*,char*);

void LED_ON(void);
void LED_OFF(void);
void LED_Init(void);

int flash_load_first_cert(char*);
int flash_load_new_cert(char*);
int flash_read_cert(void);

