#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <type.h>
#include <network.h>


#define CORE_NAME_LENGTH 64

#ifndef core_id_t
    typedef int core_id_t;
#endif



typedef struct XLcore {
    str name[CORE_NAME_LENGTH];
    core_id_t id;
    XLnet net;
}XLcore;

typedef struct XLcore_list {
    XLcore * core;
    struct XLcore_list * next;
}XLcore_list;

int core_init(void);
int core_add(XLnet * net,str * name);
void core_list(void);
int core_remove(core_id_t id);
XLcore * core_get_by_id(core_id_t id);
XLcore * core_get_by_name(str * name);

#endif
