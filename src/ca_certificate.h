#ifndef __CA_CERTIFICATE_H__
#define __CA_CERTIFICATE_H__

#define CA_CERTIFICATE_TAG 1
#define TLS_PEER_HOSTNAME "tintin"

static unsigned char ca_certificate[] = {
#include "CA.der.inc"
};

#endif /* __CA_CERTIFICATE_H__ */