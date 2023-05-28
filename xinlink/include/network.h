#ifndef NETWORK_H
#define NETWORK_H

#include <malloc.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <type.h>
#include <instruction.h>

#define SEND_QUEUE 1
#define RECIVE_QUEUE 2

#define PAK_MAX_SIZE 256

#define NETWORK_MODE_CONNECT_REQUEST 32768
#define NETWORK_MODE_CONNECT_ACCEPT 36864

#define NETWORK_MODE_SENDER_EVENT_ID 0
#define NETWORK_MODE_SENDER_PERMIT 4
#define SEE_SENDER_ONLY 12

#define NETWORK_MODE_RECEIVER_EVENT_ID 0
#define NETWORK_MODE_RECEIVER_PERMIT 1
#define NETWORK_MODE_RECEIVER_START_EVENT 2
#define SEE_RECEIVER_ONLY 3

XLnet * network_get_local_info(void);
XLins * ins_decode_data(DATA *data);

int send_queue_del(void);
int send_queue_add(XLins * ins,LEVEL level);
void send_queue_show(void);

int recv_queue_del(void);
int recv_queue_add(XLins * ins,LEVEL level);
void recv_queue_show(void);


void * ins_recv_thread(void *arg);
void *ins_send_thread(void* arg);
int network_thread_init(void);

DATA * connect_make_data(core_id_t core_id,int req_or_acc,uint16_t pak_id);
core_id_t connect_decode_data_and_connect(DATA * data);

#endif // NETWORK_H
