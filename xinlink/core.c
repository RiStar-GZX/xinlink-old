#include <core.h>

XLcore_list core_list_head;

int core_init(void)
{
    if(core_list_head.core!=NULL||core_list_head.next!=NULL)return -1;
    core_list_head.core=malloc(sizeof (XLcore));
    core_list_head.next=NULL;
    core_list_head.core->id=1;
    core_list_head.core->net=*network_get_local_info();
    strcpy(core_list_head.core->name,"core1");
    return 1;
}


int xinlink_init(void)
{
    if(core_init()<=0)return -1;
    if(network_thread_init()<=0)return -1;
    return 1;
}

int core_add(XLnet * net,str * name)
{
    int id=1,mode=0;
    if(net->port==0)return -1;
    extern XLcore_list core_list_head;
    XLcore_list * core_now=&core_list_head,*core_front=core_now;
    while (1) {
        if(strcmp(core_now->core->name,name)==0)return -1;
        if(core_now->next==NULL)break;
        core_now=core_now->next;
    }
    core_now=&core_list_head;
    printf("aaa\n");
    while (1)
    {
        if(id==core_now->core->id)id++;
        else if(core_now->core->id!=id)
        {
            mode=1;
            break;
        }
        if(core_now->next==NULL)break;
        core_front=core_now;
        core_now=core_now->next;
    }

    XLcore_list * core_new=malloc(sizeof (XLcore_list));
    core_new->core=malloc(sizeof (XLcore));
    strcpy(core_new->core->name,name);
    core_new->core->net=*net;
    core_new->core->id=id;

    if(mode==1)
    {
        core_front->next=core_new;
        core_new->next=core_now;
    }
    else
    {
        core_now->next=core_new;
        core_new->next=NULL;
    }
    return  id;
}

void core_list(void)
{
    printf("core list:");
    extern XLcore_list core_list_head;
    XLcore_list * core_now=&core_list_head;
    while (1) {
        printf("%d ",core_now->core->id);
        if(core_now->next==NULL)break;
        core_now=core_now->next;
    }
    printf("\n");
}

int core_remove(core_id_t id)
{
    if(id==1)return -1;
    extern XLcore_list core_list_head;
    XLcore_list * core_now=&core_list_head,*core_front=core_now;
    while (1) {
        if(id==core_now->core->id)
        {
            core_front->next=core_now->next;
            free(core_now->core);
            free(core_now);
            return 1;
        }
        if(core_now->next==NULL)break;
        core_front=core_now;
        core_now=core_now->next;
    }
    printf("no!\n");
    return  -2;
}

XLcore * core_get_by_id(core_id_t id)
{
    if(id==0)return NULL;
    extern XLcore_list core_list_head;
    XLcore_list * core_now=&core_list_head;
    if(core_now==NULL)return NULL;
    while (1) {
        if(id==core_now->core->id)return core_now->core;
        if(core_now->next==NULL)return NULL;
        core_now=core_now->next;
    }
    return NULL;
}

XLcore * core_get_by_net(XLnet * net)
{
    if(net==NULL)return NULL;
    extern XLcore_list core_list_head;
    XLcore_list * core_now=&core_list_head;
    if(core_now==NULL)return NULL;
    while (1) {
        if(net->ip==core_now->core->net.ip&&net->port==core_now->core->net.port){
            return core_now->core;}
        if(core_now->next==NULL)return NULL;
        core_now=core_now->next;
    }
    return NULL;
}

XLcore * core_get_by_name(str * name)
{
    extern XLcore_list core_list_head;
    XLcore_list * core_now=&core_list_head;
    if(core_now==NULL)return NULL;
    while (1) {
        if(strcmp(name,core_now->core->name)==0)return core_now->core;
        if(core_now->next==NULL)return NULL;
        core_now=core_now->next;
    }
    return NULL;
}

int core_rename(core_id_t id,str * name)
{
    if(id==0)return -1;
    extern XLcore_list core_list_head;
    XLcore_list * core_now=&core_list_head;
    if(core_now==NULL)return -1;
    while (1) {
        if(strcmp(core_now->core->name,name)==0)
        {
            //name is same
            if(id==core_now->core->id)return 2;
            else return -1;
        }
        if(core_now->next==NULL)break;
        core_now=core_now->next;
    }
    XLcore * core=core_get_by_id(id);
    if(core==NULL)return -1;
    strcpy((str*)core->name,name);
    return 1;
}
