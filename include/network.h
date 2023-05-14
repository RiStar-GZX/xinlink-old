#ifndef NETWORK_H
#define NETWORK_H

#include <malloc.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <type.h>
#include <instruction.h>

#define SEND_QUEUE 1
#define RECIVE_QUEUE 2

#define PAK_MAX_SIZE 256

#define NETWORK_MODE_CORE_CONNECT_REQUIRE 0
#define NETWORK_MODE_CORE_CONNECT_ACCEPT 1
#define NETWORK_MODE_START_INS 2
#define NETWORK_MODE_COMMON_INS 3

XLnet * network_get_local_info(void);

int send_queue_del(void);
int send_queue_add(XLins * ins,LEVEL level);
void send_queue_show(void);

int recv_queue_del(void);
int recv_queue_add(XLins * ins,LEVEL level);
void recv_queue_show(void);


void send_ins_thread(void);
#endif // NETWORK_H
