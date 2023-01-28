#ifndef NETWORK_H
#define NETWORK_H

#include <signal.h>
#include <device.h>
#include <type.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <pthread.h>

#define DATA_END 128
#define PAK_END 255
#define PAK_SIZE 128
void show_d(uint8_t *data,uint32_t datasize);
int16_t du_pak_size(uint8_t *data);
pthread_t net_init();
int8_t net_send_pak(netdev_id_t netdev_id,XLsig_pak * pak);
int8_t net_get_pak(uint8_t * data,uint32_t datasize,netdev_id_t * netdev_id,XLsig_pak * pake);
#endif
