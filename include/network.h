#ifndef NETWORK_H
#define NETWORK_H

#include <signal.h>
#include <device.h>
#include<network.h>
#include <malloc.h>
#include <type.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <core.h>
#include <arpa/inet.h>
 #include <unistd.h>
#include <pthread.h>

#define PAK_PAR_DATA_END 128
#define PAK_DATA_END 255
#define PAK_DATA_SIZE 128
#define PAK_MODE_SIGNAL 1
#define PAK_MODE_CONNECT 0
#define PAK_MODE_SEARCH 2
#define PAK_MODE_CONNECT 3
#define PAK_MODE_ACCEPT_CONNECT 4
#define PAK_MAX_SIZE 256

int add_data(void * pak,int * p,void * data,int datasize);

void show_par_data(uint8_t *data,uint32_t datasize);
int du_buf_size(uint8_t *data);
int net_init();
int net_receive(void);
int net_send(XLnet * net,uint8_t * data,uint32_t size);
int net_send_sig(netdev_id_t netdev_id,XLsig_pak * pak);
int net_get_sig(uint8_t * data,uint32_t datasize,dev_id_t * dev_id,XLsig_pak * pak);
int net_broadcast_send (uint16_t port,void *buf,uint32_t bufsize);
int net_broadcast_receive(void);
int net_send_core(void);
int net_get_core(void * buf,XLnet * net);
#endif
