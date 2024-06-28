#ifndef __CA_CERTIFICATE_H__
#define __CA_CERTIFICATE_H__

#define CA_CERTIFICATE_TAG 1
#define TLS_PEER_HOSTNAME "tintin"

static const unsigned char ca_certificate[] = {
#include "CA.der.inc"
};
// #if defined(CONFIG_MBEDTLS_KEY_EXCHANGE_PSK_ENABLED)
// #include CONFIG_NET_SAMPLE_PSK_HEADER_FILE
// #endif

#endif /* __CA_CERTIFICATE_H__ */