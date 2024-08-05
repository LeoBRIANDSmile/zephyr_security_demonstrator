#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
#include "common.h"

#define CERT_PATH "/certs/CA.pem"
#define REQUEST_NEW_CERT "GET " CERT_PATH " HTTP/1.0\r\nHost: " HTTP_HOST "\r\n\r\n"

int http_get_new_cert(char* new_cert){
    Socket_Init();
    Socket_Send(REQUEST_NEW_CERT);
    Socket_Receive_to_tab(new_cert);
    Socket_Close();
    return 1;
}
