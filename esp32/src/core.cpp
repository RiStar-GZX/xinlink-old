#include <core.h>

XLcore_list core_list_head;

int core_init(void)
{
    if(core_list_head.core!=NULL||core_list_head.next!=NULL)return -1;
    core_list_head.core=(XLcore*)malloc(sizeof (XLcore));
    core_list_head.next=NULL;
    core_list_head.core->id=1;
    core_list_head.core->net=network_get_local_info();
    core_list_head.core->safety=CORE_STATE_VERIFIED;
    strcpy(core_list_head.core->name,"core1");
    return 1;
}


int xinlink_init(void)
{
    if(core_init()<=0)return -1;
    queue_init();
    if(network_thread_init()<=0)return -1;
    return 1;
}

int core_add(XLnet * net,char * name)
{
    if(net==NULL||name==NULL)return -1;
    int id=1,mode=0;
    if(net->port==0)return -1;
    extern XLcore_list core_list_head;
    XLcore_list * core_now=&core_list_head,*core_front=core_now;
    while (1) {
        if(core_now->core->net.ip==net->ip){
            strcpy(core_now->core->name,name);
            return -2;
        }
        if(core_now->next==NULL)break;
        core_now=core_now->next;
    }
    core_now=&core_list_head;
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

    XLcore_list * core_new=(XLcore_list *)malloc(sizeof (XLcore_list));
    core_new->core=(XLcore*)malloc(sizeof (XLcore));
    strcpy(core_new->core->name,name);
    core_new->core->net=*net;
    core_new->core->id=id;
    core_new->core->safety=CORE_STATE_UNVERIFIED;
    core_new->core->sign_list=NULL;
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
    printf("core info\n");
    extern XLcore_list core_list_head;
    XLcore_list * core_now=&core_list_head;
    while (1) {
        printf("CORE_NAME:%s | CORE_ID:%d",core_now->core->name,core_now->core->id);
        if(core_now->core->id==1)printf(" | CORE_STATE:LOCAL\n");
        else if(core_now->core->safety==CORE_STATE_UNVERIFIED)printf(" | CORE_STATE:UNVERIFIED\n");
        else if(core_now->core->safety==CORE_STATE_VERIFIED)printf(" | CORE_STATE:VERIFIED\n");
        else if(core_now->core->safety==CORE_STATE_WAITTING)printf(" | CORE_STATE:WAITTING\n");
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
        if(net->ip==core_now->core->net.ip/*&&net->port==core_now->core->net.port*/){
            return core_now->core;}
        if(core_now->next==NULL)return NULL;
        core_now=core_now->next;
    }
    return NULL;
}

XLcore * core_get_by_ip(IP ip){
    extern XLcore_list core_list_head;
    XLcore_list * core_now=&core_list_head;
    if(core_now==NULL)return NULL;
    while (1) {
        if(ip==core_now->core->net.ip){
            return core_now->core;}
        if(core_now->next==NULL)return NULL;
        core_now=core_now->next;
    }
    return NULL;
}

XLcore * core_get_by_name(char * name)
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

int core_rename(core_id_t id,char * name)
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
    strcpy((char*)core->name,name);
    return 1;
}

int core_add_sign(core_id_t core_id,XLpak_signinfo * sign_list,int mode){
    if(sign_list==NULL||core_id!=CORE_MYSELF_ID)return -1;
    XLcore * core=core_get_by_id(core_id);
    if(core==NULL)return -1;
    XLpak_signinfo * list_now=core->sign_list,*next;
    if(mode==0){
        while(list_now!=NULL){
            next=list_now->next;
            free(list_now);
            list_now=next;
        }
        core->sign_list=sign_list;
        return 1;
    }
    if(mode==1){
        while(list_now!=NULL){
            if(list_now->next==NULL){
                list_now->next=sign_list;
                return 1;
            }
        }
        core->sign_list=sign_list;
        return 1;
    }
    return -1;
}
/*
int core_remove_sign(core_id_t core_id,char * name){
    if(core_id==CORE_MYSELF_ID)return -1;
    return 1;
}
*/

int core_sign_remove_all(core_id_t core_id){
    if(core_id==CORE_MYSELF_ID)return -1;
    XLcore * core=core_get_by_id(core_id);
    if(core==NULL)return -1;
    XLpak_signinfo * list_now=core->sign_list,*next;
    while(list_now!=NULL){
        next=list_now->next;
        free(list_now);
        list_now=next;
    }
    core->sign_list=NULL;
    return 1;
}

XLpak_signinfo *  core_get_sign(core_id_t core_id){
    if(core_id==CORE_MYSELF_ID){
        extern XLevent_list * event_list_head;
        XLevent_list * event_now=event_list_head;
        XLpak_signinfo * sign_now=NULL;
        while(event_now!=NULL){
            if(event_now->event.sign.use==ENABLE){
                if(sign_now==NULL){
                    sign_now=(XLpak_signinfo*)malloc(sizeof(XLpak_signinfo));
                    sign_now->name=event_now->event.sign.name;
                    sign_now->type=event_now->event.sign.type;
                    sign_now->next=NULL;
                }
                else {
                    XLpak_signinfo * sign_new=(XLpak_signinfo*)malloc(sizeof(XLpak_signinfo));
                    sign_new->name=event_now->event.sign.name;
                    sign_new->type=event_now->event.sign.type;
                    sign_new->next=NULL;
                    sign_now->next=sign_new;
                    sign_now=sign_new;
                }
                printf("\nname:%s\ntype:%s\n",sign_now->name,sign_now->type);
            }
            event_now=event_now->next;
        }

        return sign_now;
    }

    return NULL;
    //XLcore * core=core_get_by_id(core_id);
    //if(core==NULL)return NULL;
    //return core->sign_list;
}
