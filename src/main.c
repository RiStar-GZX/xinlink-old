#include <stdio.h>
#include <core.h>
#include <event.h>
#include <network.h>
#include <instruction.h>
#include <string.h>

int main()
{
    core_init();
    XLins ins;
    INS * inss="event ass";
    ins.ins=inss;
    //strcpy(ins.ins,"event a");
    send_queue_add(&ins,15);


    send_queue_add(&ins,1);

    send_queue_add(&ins,5);

    send_queue_add(&ins,5);
    send_queue_show();
    send_ins_thread();
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
