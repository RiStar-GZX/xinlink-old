#ifndef NETWORK_H
#define NETWORK_H

#include <malloc.h>

#include <type.h>
#include <arpa/inet.h>

#ifndef IP
typedef uint32_t IP;
#endif

#ifndef MAC
typedef uint8_t MAC;
#endif

#ifndef port

typedef uint16_t PORT;
#endif

typedef struct XLnet {
    IP ip;
    MAC mac[6];
    PORT port;
}XLnet;

XLnet * network_get_local_info(void);

#endif // NETWORK_H
