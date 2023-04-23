#include <stdio.h>
#include <core.h>
#include <event.h>

XLcmd_list * myevent(XLcmd_list cmd_list)
{
    printf("this is event!\n\n");
    return NULL;
}

int main(int argc, char *argv[])
{
    event_add("event",myevent);
    XLcmd_list cmd;
    if(event("event",cmd)==NULL)printf("NULL\n");
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
