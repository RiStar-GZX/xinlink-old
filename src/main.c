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

    XLins  ins;
    while(1)
    {
        int a;
        printf("mode:");
        scanf("%d",&a);
        recv_queue_add(&ins,(LEVEL)a);
        recv_queue_show();
    }
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

    //XLins_decoded ins=*ins_decode("event 55 4 idf");
    //printf("%s\n",ins_encode(&ins));
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
