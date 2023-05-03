#ifndef NETWORK_H
#define NETWORK_H

#include <malloc.h>
#include <arpa/inet.h>

#include <type.h>
#include <instruction.h>

#define SEND_QUEUE 1
#define RECIVE_QUEUE 2



#ifndef IP
typedef uint32_t IP;
#endif

#ifndef MAC
typedef uint8_t MAC;
#endif

#ifndef PORT

typedef uint16_t PORT;
#endif

#ifndef LEVEL

typedef uint8_t LEVEL;
#endif


typedef struct XLnet {
    IP ip;
    MAC mac[6];
    PORT port;
}XLnet;

typedef struct XLins_queue {
    XLins * ins;
    LEVEL level;
    struct XLins_queue * front;
    struct XLins_queue * next;
}XLins_queue;

XLnet * network_get_local_info(void);

int send_queue_del(void);
int send_queue_add(XLins * ins,LEVEL level);
void send_queue_show(void);

int recv_queue_del(void);
int recv_queue_add(XLins * ins,LEVEL level);
void recv_queue_show(void);

#endif // NETWORK_H
