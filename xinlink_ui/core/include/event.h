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
enum SIGN_MODE{
    SIGN_CS_SERVER =1,
    SIGN_CS_CLIENT,
    SIGN_CS_BOTH
};

enum operate_code{
    OPERATE_CODE_CONNECT =1,
    OPERATE_CODE_DISCONNECT,
    OPERATE_CODE_INS ,
};


int app_add(const char* name,EVENT init,EVENT event,EVENT exit);
void app_show(void);
int app_remove(app_id_t id);
int app_set(const char* name,EVENT event);
XLapp * app_get_by_id(app_id_t id);
XLapp * app_get_by_name(const char* name);
INS *app(INS *ins);

event_id_t event_create(EVENT init,EVENT event,EVENT exit);
XLevent * event_get_by_id(event_id_t id);

int event_run(event_id_t id);
void event_show(void);
event_id_t event_create_and_run(const char* app_name);

int event_add_sign(event_id_t event_id,const char* sign_name,const char* type,uint8_t mode);
event_id_t event_create_run_add_sign(const char*app_name,const char* sign_name,const char*type,uint8_t mode);
int event_remove_sign(event_id_t event_id);
event_id_t event_get_by_signname(const char* sign_name);
XLsign *event_get_sign(event_id_t event_id);

#endif // EVENT_H
