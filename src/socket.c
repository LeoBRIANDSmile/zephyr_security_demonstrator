#include <zephyr/net/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "common.h"
#include "ca_certificate.h"
#include <zephyr/net/tls_credentials.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/kernel.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/devicetree.h>
// #include <zephyr/pm_config.h>

LOG_MODULE_REGISTER(socket, LOG_LEVEL_DBG);

#define IP_ADDRESS "172.20.10.3"

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

// Typedefs
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

SOCKET sock;

int Socket_Init(void){
	static int flag = 0;
	SOCKADDR_IN mysin = { 0 };

	int ret = -1, count = 0;

	if (!flag){
		// Add credentials
		char *cert = flash_read_cert(0);

		// LOG_HEXDUMP_INF(cert, 779, "CERT BUFFER CONTENT");

		#if defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)
			ret = tls_credential_add(CA_CERTIFICATE_TAG, TLS_CREDENTIAL_CA_CERTIFICATE, cert, CERT_SIZE);
			if (ret < 0) {
				LOG_ERR("Error during credentials registration");
				return 0;
			}
		#endif

		free(cert);

		flag++;
	}

	// Socket Creation
	#if defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)
	    sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TLS_1_2);
	#else
	    sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	#endif

	if (sock == INVALID_SOCKET) {
		LOG_ERR("Error during socket creation");
		return 0;
	}
	else {
		LOG_INF("Created socket : %d", sock);
	}

	// Socket Configuration
	net_addr_pton(AF_INET,IP_ADDRESS,&mysin.sin_addr);
	mysin.sin_port = htons(HTTP_PORT);
	mysin.sin_family = AF_INET;

	#if defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)

		// Remove offloading of TLS stack
		struct ifreq ifreq = {
			.ifr_name = "wlan0"
		};

		int tls_native = 1;
		ret = zsock_setsockopt(sock, SOL_TLS, TLS_NATIVE, &tls_native, sizeof(tls_native));
		if(ret < 0){
			LOG_ERR("Failed to set TLS option");
			return 0;
		}
		
		ret = zsock_setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, &ifreq, sizeof(ifreq));
		if(ret < 0){
			LOG_ERR("Failed to bind socket to device, setsockopt returns : %d", ret);
			LOG_ERR("Errno : %s",strerror(errno));
			return 0;
		}

		// Set peer verification to : 0 (none), 1 (optionnal), 2 (required)
		int peer_verify = 2;
		ret = zsock_setsockopt(sock, SOL_TLS, TLS_PEER_VERIFY, &peer_verify, sizeof(peer_verify));
		if (ret < 0) {
			LOG_ERR("Error during socket creation");
			return 0;
		}
		
		// Add tag(s) that could be certificate, private key, pre-shared key... (Cf Add credentials)
		sec_tag_t sec_tag_list[] = { CA_CERTIFICATE_TAG };

		ret = zsock_setsockopt(sock, SOL_TLS, TLS_SEC_TAG_LIST,sec_tag_list, sizeof(sec_tag_list));
		if (ret < 0) {
			LOG_ERR("Failed to set TLS_SEC_TAG_LIST (rc %d, errno %d)", ret, errno);
			return 0;
		}

		ret = zsock_setsockopt(sock, SOL_TLS, TLS_HOSTNAME, TLS_PEER_HOSTNAME, sizeof(TLS_PEER_HOSTNAME));
		if (ret < 0) {
			LOG_ERR("Failed to set TLS_HOSTNAME option (rc %d, errno %d)", ret, errno);
			return 0;
		}

	#endif
	
	// Socket Connection
	while(zsock_connect(sock,(SOCKADDR *) &mysin, sizeof(SOCKADDR))<0){
		count++;
		LOG_WRN("Retrying to connect...");
	    k_sleep(K_MSEC(1000));
		if(count>=15){
			LOG_ERR("Cannot connect");
			return 0;
		}
	}
	LOG_INF("Connection success");
	return 1;
}

int Socket_Send(char * data){
    if(zsock_send(sock, data, strlen(data), 0) < 0)
	{
		LOG_ERR("Send error (send())");
		return 0;
	}
	else{
		LOG_INF("Send succeeded, message : '%s' (send())",data);
		k_sleep(K_MSEC(2000));
		return 1;
	}
}

int Socket_Receive(char* data){
	static int n = 0,total=0;
	printf("\r\n\r\nData received : \r\n\r\n");
	while (1) {
		n = zsock_recv(sock, data, MAX_SIZE_BUFFER - 1, 0);
		total+=n;
		if (n < 0) {
			// LOG_ERR("Error reading response");
			break;
		}
		if (n == 0) {
			break;
		}
		data[n] = 0;
		printf("%s", data);
		//Remplacer printf par écriture en flash
	}
	printf("\r\n");
	printf("\r\nTotal bytes received : %d\r\n",total);
	n=0;
	total=0;
	return 1;
}

int Socket_Receive_to_tab(char* data){
	static int n = 0;
	n = zsock_recv(sock, data, MAX_SIZE_BUFFER, 0);
	printf("%s", data);
	return 1;
}

int Socket_Receive_firmware_to_flash(){
	uint32_t n = 0, remaining_size = FLASH_SECTOR_SIZE;
	int ret = 0;
	const struct device *flash_dev;
    off_t dfu_flash_offset;
	char buf_to_write[FLASH_SECTOR_SIZE];
	size_t partition_size;

	partition_size = FIXED_PARTITION_SIZE(slot1_partition);
    dfu_flash_offset = FIXED_PARTITION_OFFSET(slot1_partition);
    flash_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_flash_controller));

    LOG_INF("dfu_flash_offset_offset : 0x%x", dfu_flash_offset);
    LOG_INF("dfu_flash_offset_dev : %s", flash_dev->name);

	if (!device_is_ready(flash_dev)) {
		LOG_ERR("%s: device not ready.\n", flash_dev->name);
		return 0;
	}

	printf("\r\nErasing second slot...\r\n");

	for (int i = 0; i<partition_size; i+=FLASH_SECTOR_SIZE) {
		ret = flash_erase(flash_dev, dfu_flash_offset+i, FLASH_SECTOR_SIZE);
		if(ret<0){
			printf("\r\nError during flash erase\r\n");
		}
		progress_bar(i,partition_size-FLASH_SECTOR_SIZE);
	}
	
	printf("\r\nSecond slot erased successfully\r\n");

	printf("\r\nDownloading firmware...\r\n");

	while (1) {
		int total_bytes_received = 0;
    
    	while (total_bytes_received < FLASH_SECTOR_SIZE) {
			int bytes_remaining = FLASH_SECTOR_SIZE - total_bytes_received;
			
			n = zsock_recv(sock, buf_to_write + total_bytes_received, bytes_remaining, 0);
			
			if (n < 0) {
				return 0;
			} 
			else if (n == 0) {
				break;
			}
			
			total_bytes_received += n;
    	}
				
		ret = flash_write(flash_dev, dfu_flash_offset, buf_to_write, FLASH_SECTOR_SIZE);

		if (ret<0) {
			LOG_ERR("Error during flash write");
			printf("\r\nError during downloading\r\n");
			return 0;
		}
		dfu_flash_offset+=FLASH_SECTOR_SIZE;
		progress_bar(dfu_flash_offset,250000);
		if (0 == n) {
			break;
		}

	}
	printf("\r\nFirmware downloaded successfully\r\n\r\n");

	return 1;
}

int Socket_Close(void){
	zsock_close(sock);
	LOG_INF("Socket closed");
}