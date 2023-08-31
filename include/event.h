#ifndef EVENT_H
#define EVENT_H

#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>


#include <type.h>
#include <instruction.h>
#include <ll.h>

int app_add(char * name,EVENT event);
void app_list(void);
int app_remove(app_id_t id);
int app_set(char * name,EVENT event);
XLapp * app_get_by_id(app_id_t id);
XLapp * app_get_by_name(char * name);
INS *app(INS *ins);

event_id_t event_create(EVENT event);
XLevent * event_get_by_id(event_id_t id);

int event_run(event_id_t id);
void event_show(void);
event_id_t event_create_and_run(char * app_name);

int event_add_sign(event_id_t event_id,char * sign_name,char * type);
int event_remove_sign(event_id_t event_id);
event_id_t event_get_by_signname(char * sign_name);
XLsign *event_get_sign(event_id_t event_id);

#endif // EVENT_H
