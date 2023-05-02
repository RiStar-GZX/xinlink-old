#ifndef EVENT_H
#define EVENT_H

#include<stdio.h>
#include<malloc.h>
#include<string.h>

#include <type.h>
#include <instruction.h>

#define EVENT_NAME_LENGTH 64

#ifndef event_id_t
    typedef unsigned int      event_id_t;
#endif

typedef  INS *(*EVENT)(INS * ins);

typedef struct XLevent{
    str name[EVENT_NAME_LENGTH];
    event_id_t id;
    EVENT event;
}XLevent;

typedef struct XLevent_list{
    XLevent * event;
    struct XLevent_list * next;
}XLevent_list;

int event_add(str * name,EVENT event);
void event_list(void);
int event_remove(event_id_t id);
int event_set(str * name,EVENT event);
XLevent * event_get_by_id(event_id_t id);
XLevent * event_get_by_name(str * name);
INS *event(str *name,INS *ins);

#endif // EVENT_H
