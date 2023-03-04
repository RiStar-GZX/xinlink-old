#ifndef NETWORK_H
#define NETWORK_H

#include <signal.h>
#include <device.h>
#include <malloc.h>
#include <type.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
 #include <unistd.h>
#include <pthread.h>

#define PAK_PAR_DATA_END 128
#define PAK_DATA_END 255
#define PAK_DATA_SIZE 128
#define PAK_MODE_SIGNAL 1
#define PAK_MODE_CONNECT 0
#define PAK_MODE_SEARCH
#define PAK_MAX_SIZE 256


typedef struct XLCore_list
{
    XLnet net;
    struct XLCore_list * next;
}XLCore_list;
void * add_data(void * pak,int * p,void * data,int datasize);

void show_par_data(uint8_t *data,uint32_t datasize);
int du_buf_size(uint8_t *data);
pthread_t net_init();
int net_pthread(void);
int net_send(XLnet * net,uint8_t * data,uint32_t size);
int net_send_pak(netdev_id_t netdev_id,XLpak * pak);
int net_get_pak(uint8_t * data,uint32_t datasize,dev_id_t * dev_id,XLpak * pak);
struct XLCore_list * net_search(XLnet * net);
struct XLCore_list * net_rev_search(void);
#endif
