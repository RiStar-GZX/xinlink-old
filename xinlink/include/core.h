#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <type.h>
#include <network.h>

int core_init(void);
int xinlink_init(void);
int core_add(XLnet * net,str * name);
void core_list(void);
int core_remove(core_id_t id);
XLcore * core_get_by_id(core_id_t id);
XLcore * core_get_by_net(XLnet * net);
XLcore * core_get_by_name(str * name);

#endif
