#ifndef NETWORK_H
#define NETWORK_H

#include <signal.h>
#include <device.h>
#include <type.h>
#include <stdio.h>
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


typedef struct XLCore_list
{
    XLnet net;
    struct XLCore_list * next;
}XLCore_list;

void show_d(uint8_t *data,uint32_t datasize);
int16_t du_pak_size(uint8_t *data);
pthread_t net_init();
int8_t net_send(XLnet * net,uint8_t * data,uint32_t size);
int8_t net_get_pak(uint8_t * data,uint32_t datasize,netdev_id_t * netdev_id,XLpak * pake);
struct XLCore_list * net_search(XLnet * net);
struct XLCore_list * net_rev_search(void);
#endif
