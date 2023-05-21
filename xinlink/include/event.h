#ifndef EVENT_H
#define EVENT_H

#include<stdio.h>
#include<malloc.h>
#include<string.h>

#include <type.h>
#include <instruction.h>

int app_add(str * name,EVENT event);
void app_list(void);
int app_remove(app_id_t id);
int app_set(str * name,EVENT event);
XLapp * app_get_by_id(app_id_t id);
XLapp * app_get_by_name(str * name);
INS *app(INS *ins);

event_id_t event_create(EVENT event);
int event_run(event_id_t id,XLins * ins);

#endif // EVENT_H
