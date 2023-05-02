#include <stdio.h>
#include <core.h>
#include <event.h>
#include <network.h>
#include <instruction.h>

/*INS * myevent(INS * ins)
{
    printf("this is event!\n\n");
    return NULL;
}*/

int main()
{
    /*event_add("event",myevent);
    INS * ins;
    if(event("event",ins)==NULL)printf("NULL\n");*/

    /*XLins * ins=NULL;
    while(1)
    {
        int a;
        printf("mode:");
        scanf("%d",&a);
        send_queue_add(ins,(LEVEL)a);
        show_send_queue();
    }*/
    /*XLins_decoded uncode;
    uncode.data_list=malloc(sizeof (XLins_decoded_data));
    strcpy((str*)uncode.event_name,"event");
    strcpy((str*)uncode.data_list->data_name,"data1");
    uncode.data_list->datasize=4;
    uint8_t a[4]={0x30,0x31,0x32,0x33};
    uncode.data_list->data=(uint8_t*)&a;

    uncode.data_list->next=malloc(sizeof (XLins_decoded_data));
    strcpy((str*)uncode.data_list->next->data_name,"data2");
    uncode.data_list->next->data=(uint8_t*)&a;
    uncode.data_list->next->datasize=4;

    printf("encode:%sX\n",ins_encode(&uncode));
    printf("\n%s\n",(uint8_t*)&a);*/
    ins_decode("event ap sd dfs dfss    sdgs");
    return 0;
}


/*int main(int argc, char *argv[])
{
    while (1){
        printf("number:");
        int a;
        scanf("%d",&a);
        if(a==0){
            event_list();
        }
        else if(a==-1){
            printf(" :%d",event_add("114",NULL));
            printf("add!\n");
        }
        else {
            event_remove(a);
        }
    }
    return  0;
}*/

/*int main(int argc, char *argv[])
{
    core_init();
    while (1){
        printf("number:");
        int a;
        scanf("%d",&a);
        if(a==0){
            core_list();
        }
        else if(a==1){
            printf(" :%d",core_add(network_get_local_info(),"114"));
            printf("add!\n");
        }
        else {
            core_remove(a);
        }
    }
    return  0;
}*/
