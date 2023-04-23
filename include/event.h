#ifndef EVENT_H
#define EVENT_H

#include<stdio.h>
#include<malloc.h>
#include<string.h>

#include <type.h>
#include <command.h>

#define EVENT_NAME_LENGTH 64

#ifndef event_id_t
    typedef unsigned int      event_id_t;
#endif

typedef  XLcmd_list * (*EVENT)(XLcmd_list cmd_list);

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
XLcmd_list * event(str *name,XLcmd_list cmd_list);

#endif // EVENT_H
