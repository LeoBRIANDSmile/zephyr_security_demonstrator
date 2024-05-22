#include <zephyr/net/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "common.h"


#define IP_ADDRESS "172.20.10.2" 
#define PORT 1234

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

// Typedefs
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

SOCKET sock;
SOCKADDR_IN sin = { 0 };

void Socket_Init(void){
	
	int ret;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
	
	if(sock == INVALID_SOCKET)
	{
		printf("\r\nSocket creation error (socket())\r\n");
	}
	else{
		printf("\r\nCreated socket : %d\r\n", sock);
	}

	net_addr_pton(AF_INET,IP_ADDRESS,&sin.sin_addr);
	sin.sin_port = htons(PORT); /* on utilise htons pour le port */
	sin.sin_family = AF_INET;
    ret = connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR));
	if (ret < 0) {
		printf("Cannot connect to UDP remote");
		ret = -errno;
	}
}

void Socket_Send(char * data){
    if(send(sock, data, strlen(data), 0) < 0)
	{
		printf("\r\nSend error (send())\r\n");
        printf("%s",strerror(errno));
	}
	else{
		printf("\r\nSend succeeded, message : '%s' (send())\r\n",data);
		k_sleep(K_MSEC(2000));
	}
}

void Socket_Receive(char* data){
	static int n = 0;
	memset(data,'\0',MAX_SIZE_BUFFER);
    n = recv(sock, data, MAX_SIZE_BUFFER ,0);
	printf("\r\nReceived data (length : %d bytes):\r\n%s\r\n",n,data);
}
