#include <stdio.h>
#include <core.h>
#include <event.h>
#include <network.h>
#include <instruction.h>
#include <string.h>
INS * event1(XLins_queue_head * head){
    printf("\n\nthis is event!\n\n");
}

int main()
{
    if(xinlink_init()<=0)printf("init error!\n");
    app_add("app",event1);
    XLnet net;

    core_add(&net,"bb");
    XLins ins,ins2;
    INS * inss="event ass";
    ins.INS=inss;

    //ins.receiver.mode=
    //strcpy(ins.receiver.name,"app");
    //ins_recevier_set_appname(&ins,"app");
    ins.mode=NETWORK_MODE_RECEIVER_START_EVENT;
    //ins_sender_set_id(&ins,MODE_EVENT_ID,114);
    //ins_recevier_set_id(&ins,MODE_EVENT_ID,12);
    ins_recevier_set_appname(&ins,"app");
    ins.core_id=1;

    XLsource send=ins.sender,recv=ins.receiver;
    while (1) {


        int a;
        scanf("%d",&a);
        queue_add(queue_total(),&ins,0);
        //queue_add(queue_total(),&ins,1);
        //queue_add(queue_total(),&ins2,0);
        //queue_show(queue_total());
       // queue_show(monitor(&recv,&recv));


    }
    return 0;
}
/*
int  main(int argc, char *argv[])
{
    XLins ins;
    XLins_queue_head head;
    while (1) {


        int a;
        scanf("%d",&a);
        if(a>=0)queue_add(queue_total(),&ins,a);
        else queue_del(queue_total());
        queue_show(queue_total());
        }
    return 0;
}*/
/*
int main()
{
    EVENT event;
    event=event1;
    while (1) {

    int id=event_create(event);
    printf("%d\n",event_get_by_id(id)->id);
    event_get_by_id(id)->event(NULL);
    }
}*/

