#include <core.h>

XLCore core_local;

int core_init(void)
{
    core_local.id=1;
    core_local.next=NULL;
    core_local.net.family=NETWORK_IPV4;
    core_local.net.ip.net_ipv4=inet_addr("192.168.1.23");
    core_local.net.port=8181;
    return 1;
}

XLCore * core_get(core_id_t core_id)
{
    extern XLCore core_local;
    if(core_id==1)return &core_local;
    return NULL;
}
int core_check_repeat(XLnet * net)
{
    if(net==NULL)return 0;
    extern XLCore core_local;
    XLCore *core_now=&core_local;
    for(;;) {
        if(core_now->net.ip.net_ipv4==net->ip.net_ipv4)return 0;
        if(core_now->next==NULL)return 1;
        core_now=core_now->next;
    }
}

core_id_t core_create(XLnet * net)
{
    if(core_check_repeat(net)!=1)
    {
        printf("创建了一个重复的核心!\n");
        return  0;
    }
    printf("成功创建了一个核心!\n");
    if(net==NULL)return 0;
    if(net->ip.net_ipv4==0||net->port==0)return 0;
    extern XLCore core_local;
    XLCore * core_now=&core_local,*core_front=&core_local,*core_new;
    core_id_t id_new=1;
    int i=0;
    for(;;) {

        if(id_new==core_now->id){id_new++;i=1;}
        else if(i==1){
            core_new=malloc(sizeof(XLCore));
            core_new->next=core_front->next;
            core_front->next=core_new;
            core_new->id=id_new;
            i=0;
            break;
        }
        if(core_now->next==NULL){
            core_now->next=malloc(sizeof(XLCore));
            core_now=core_now->next;
            core_now->next=NULL;
            core_now->net=*net;
            core_now->id=id_new;
            break;
        }
        core_front=core_now;
        core_now=core_now->next;
    }
    return id_new;
}

int core_del(core_id_t core_id)
{
    if(core_id==0||core_id==1)return 0;
    extern XLCore core_local;
    XLCore * core_now=&core_local,* core_front=&core_local;
    for(;;) {
        if(core_id==core_now->id){
            if(core_now->next!=NULL)core_front->next=core_now->next;
            else core_front->next=NULL;
            free(core_now);
            return 1;
        }
        if(core_now->next==NULL)return 0;
        core_front=core_now;
        core_now=core_now->next;
    }
    return 1;
}
int core_show(void)
{
    extern XLCore core_local;
    XLCore * core_now=&core_local;
    for(;;)
    {
        printf(" %d",core_now->id);
        if(core_now->next==NULL)return 0;
        core_now=core_now->next;
    }
}
