#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <type.h>
#include <event.h>
#include <network.h>
#include <instruction.h>
#include <ll.h>

#define CORE_MYSELF_ID 1
#define CORE_STATE_UNVERIFIED 0
#define CORE_STATE_VERIFIED 1
#define CORE_STATE_WAITTING 2

int core_init(void);
int xinlink_init(void);
int core_add(XLnet * net,const char* name);
void core_show(void);
int core_remove(core_id_t id);
XLcore * core_get_by_id(core_id_t id);
XLcore * core_get_by_net(XLnet * net);
XLcore * core_get_by_ip(IP ip);
XLcore * core_get_by_name(const char* name);
int core_add_sign(core_id_t core_id,XLll * sign_ll,int mode);
XLll *core_get_sign_ll(core_id_t core_id);
#endif
