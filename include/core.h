#ifndef CORE_H
#define CORE_H

#include<signal.h>
#include<device.h>
#include<type.h>
#include<link.h>
#include<network.h>
#include<xinlink.h>

//typedef uint16_t core_id_t;

typedef struct XLCore{
    core_id_t id;
    XLnet net;
    struct XLCore * next;
}XLCore;

int core_init(void);
XLCore * core_get(core_id_t core_id);
XLCore * core_get_by_ip(uint32_t ipv4);
core_id_t core_create(XLnet * net);
int core_del(core_id_t core_id);
int core_show(void);
#endif
