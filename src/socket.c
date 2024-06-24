#include <zephyr/net/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "common.h"
#include "ca_certificate.h"
#include <zephyr/net/tls_credentials.h>
// #include <mbedtls/ssl_ciphersuites.h>
// #include <mbedtls/ssl.h>


#define IP_ADDRESS "<IPADDRESS>" 
#define PORT 443

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

// Typedefs
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

SOCKET sock;
SOCKADDR_IN mysin = { 0 };

void Socket_Init(void){

	int ret = -1;
	int tls_native = 1;

	#if defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)
		ret = tls_credential_add(CA_CERTIFICATE_TAG, TLS_CREDENTIAL_CA_CERTIFICATE, ca_certificate, sizeof(ca_certificate));
		if (ret < 0) {
			printf("\r\nFailed to register public certificate: %d\r\n", ret);
		}
	#endif

	// Socket Creation
	#if defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)
	    sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TLS_1_2);
	#else
	    sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	#endif
	
	if(sock == INVALID_SOCKET)
	{
		printf("\r\nSocket creation error (socket())\r\n");
	}
	else{
		printf("\r\nCreated socket : %d\r\n", sock);
	}

	// Socket Configuration
	net_addr_pton(AF_INET,IP_ADDRESS,&mysin.sin_addr);
	mysin.sin_port = htons(PORT);
	mysin.sin_family = AF_INET;

	#if defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)

		// Remove offloading of TLS stack
		struct ifreq ifreq = {
			.ifr_name = "wlan0"
		};

		ret = zsock_setsockopt(sock, SOL_TLS, TLS_NATIVE, &tls_native, sizeof(tls_native));
		if(ret < 0){
			printf("\r\nFailed to set TLS option\r\n");
		}
		
		ret = zsock_setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, &ifreq, sizeof(ifreq));
		if(ret < 0){
			printf("\r\nFailed to bind socket to device, setsockopt returns : %d\r\n", ret);
			printf("\r\nErrno : %s\r\n",strerror(errno));
		}
		
		sec_tag_t sec_tag_list[] = { CA_CERTIFICATE_TAG };

		ret = zsock_setsockopt(sock, SOL_TLS, TLS_SEC_TAG_LIST,sec_tag_list, sizeof(sec_tag_list));
		if (ret < 0) {
			printf("Failed to set TLS_SEC_TAG_LIST (rc %d, errno %d)", ret, errno);
		}

	#endif

	// Socket Connection
	ret = zsock_connect(sock,(SOCKADDR *) &mysin, sizeof(SOCKADDR));
	if (ret < 0) {
		printf("\r\nCannot connect\r\n");
		ret = -errno;
	}
}

void Socket_Send(char * data){
    if(zsock_send(sock, data, strlen(data), 0) < 0)
	{
		printf("\r\nSend error (send())\r\n");
		printf("\r\n%s\r\n",strerror(errno));
	}
	else{
		printf("\r\nSend succeeded, message : '%s' (send())\r\n",data);
		k_sleep(K_MSEC(2000));
	}
}

void Socket_Receive(char* data){
	static int n = 0;
	memset(data,'\0',MAX_SIZE_BUFFER);
	n = zsock_recv(sock, data, MAX_SIZE_BUFFER ,0);
	printf("\r\nReceived data (length : %d bytes):\r\n%s\r\n",n,data);
}
