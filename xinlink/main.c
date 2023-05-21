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
    if(xinlink_init()<=0)printf("init error!\n");
    XLins ins;
    INS * inss="event ass";
    ins.INS=inss;
    ins.mode=NETWORK_MODE_RECEIVER_START_EVENT+NETWORK_MODE_SENDER_NOTHING;
    ins.recv_event_id=114;
    ins.core_id=1;
    //strcpy(ins.ins,"event a");

    send_queue_add(&ins,15);


 //   send_queue_add(&ins,1);

   // send_queue_add(&ins,5);

    //send_queue_add(&ins,5);
    app_add("event",event1);
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
    return 0;
}
