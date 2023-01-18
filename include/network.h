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
pthread_t net_init();
int8_t net_send_pak(netdev_op_t netdev_op,XLsig_pak * pak);
int8_t net_get_pak(uint8_t * data,uint32_t datasize,netdev_op_t * netdev_op,XLsig_pak * pake);
#endif
