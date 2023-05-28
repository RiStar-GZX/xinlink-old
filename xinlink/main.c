#include <stdio.h>
#include <core.h>
#include <event.h>
#include <network.h>
#include <instruction.h>
#include <string.h>
INS * event1(INS * ins){
    printf("\n\nthis is event!\n\n");
}
int main()
{
    /*if(xinlink_init()<=0)printf("init error!\n");
    app_add("app",event1);
    XLnet net;

    core_add(&net,"bb");
    XLins ins;
    INS * inss="event ass";
    ins.INS=inss;
    strcpy(ins.recv_app_name,"app");
    ins.mode=NETWORK_MODE_RECEIVER_START_EVENT;
    ins.recv_event_id=114;
    ins.core_id=1;

    //strcpy(ins.ins,"event a");

    while (1) {


    send_queue_add(&ins,15);
    int a;
    scanf("%d",&a);
    }

 //   send_queue_add(&ins,1);

   // send_queue_add(&ins,5);

    //send_queue_add(&ins,5);
    while (1) {

    }
    /*XLins  ins;
    while(1)
    {
        int a;
        printf("mode:");
        scanf("%d",&a);
        if(a!=0)send_queue_add(&ins,(LEVEL)a);
        else send_queue_del();
        send_queue_show();a
    }*/
    core_init();
    connect_make_data(1,0,0x5050);
    return 0;
}
