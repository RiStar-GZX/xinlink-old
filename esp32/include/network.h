#ifndef NETWORK_H
#define NETWORK_H

#include <malloc.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <instruction.h>
#include <type.h>

#ifdef PLATFORM_LINUX
#include <pthread.h>
#include <arpa/inet.h>
#endif

#ifdef PLATFORM_ESP32
#include <WiFi.h>
#include <WiFiUdp.h>
#include <sys/socket.h>
#endif

#define SEND_QUEUE          1
#define RECIVE_QUEUE        2

#define PAK_MAX_SIZE        256

#define CONNECT_REQUEST     1
#define CONNECT_ACCPET      0

#define NETWORK_MODE_INS        1                  //0001 0000 ---- ----
#define NETWORK_MODE_CONNECT    2                  //0010 0000 0000 0000
#define NETWORK_MODE_SIGN       3                  //0010 0000 0000 0000

//CONNECT
#define PAK_SEND_CORE           0        //0010 0000 0000 0000
#define PAK_SEND_CORE_REQUIRE   1        //0010 0000 0000 0001
#define PAK_CONNECT_REQUEST     2        //0010 0000 0000 0010
#define PAK_CONNECT_ACCEPT      3        //0010 0000 0000 0011
#define PAK_CONNECT_DISACCEPT   4        //0010 0000 0000 0100
#define PAK_DISCONNECT          5        //0010 0000 0000 0101

#define SEE_CONNECT_ONLY        15       //0010 0000 0000 0000

//SOURCE
#define EVENT_ID    1              //0001 0000 0001 ----
#define SIGN_NAME   2              //0001 0000 0010 ----
#define ACCESS      3              //0001 0000 0011 ----
#define START_APP   4              //0001 0000 0100 ----

//SIGN

#define SIGN_REQUIRE    1          //---- ---- ---- 0001
#define SIGN_SEND       2          //---- ---- ---- 0010
#define SEE_FOUR_ONLY        15       //0010 0000 0000 0000
//QUEUE
#define QUEUE_TYPE_NONE     0
#define QUEUE_TYPE_INS      1
#define QUEUE_TYPE_CONNECT  2
#define QUEUE_TYPE_SIGN     3

#define NETWORK_BROADCAST_PORT 8088

XLnet network_get_local_info(void);
int TCP_send(XLnet * net,DATA * data,int datasize);
int Broadcast_send (void *buf,int bufsize);

int queue_del_head(XLqueue_head *head);
int queue_del_queue(XLqueue_head *head,XLqueue * queue);
int queue_remove_all(XLqueue_head * head);
int queue_add(int mode,XLqueue_head * head,XLqueue_in * in,LEVEL level);
int queue_add_ins(XLqueue_head * head,XLpak_ins * ins,LEVEL level);
int queue_add_connect(XLqueue_head * head,XLpak_connect * connect,LEVEL level);
void queue_show(XLqueue_head * head);
XLqueue_head * queue_total(void);
XLqueue_head * queue_send(void);
XLqueue_head * queue_broadcast(void);
XLqueue_head * queue_recv(void);
int queue_init(void);

int network_thread_init(void);

XLpak_ins * buf_to_pak_ins(DATA * buf);
DATA * pak_ins_to_buf(XLpak_ins *pak_ins,int * size);
XLpak_connect * buf_to_pak_connect(DATA* buf);
DATA * pak_connect_to_buf(XLpak_connect * pak_connect,int * size);
DATA * pak_sign_to_buf(XLpak_sign * pak_sign,int * size);
XLpak_sign * buf_to_pak_sign(DATA * buf);

int network_core_find_send(void);
int network_core_connect(XLpak_connect * pak_connect);
int network_core_connect_require_send(core_id_t core_id);
int network_core_disconnect_send(core_id_t core_id);

//int network_ins_send(XLsource * sender,XLsource *receiver,core_id_t receiver_core,INS *ins);
//int network_ins_send(int receiver,INS * ins);
int network_safe(XLnet *net);
int network_ins(XLsource * sender,XLsource *receiver,INS * ins);
int network_send_sign(core_id_t core_id);
int network_require_sign(core_id_t core_id);

int network_core_find_receive(XLpak_connect * pak_connect);
int network_core_connect_require_receive(XLpak_connect * pak_connect);
#endif // NETWORK_H
