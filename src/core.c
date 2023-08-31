#include <core.h>

//XLcore_list core_list_head;
XLll * core_ll;

int core_init(void)
{
    extern XLll *core_ll;
    if(core_ll!=NULL)return -1;
    core_ll=ll_create(sizeof(XLcore));
    XLcore core_new;
    core_new.id=1;
    core_new.net=network_get_local_info();
    core_new.safety=CORE_STATE_VERIFIED;
    strcpy(core_new.name,"core1");
    ll_add_member_head(core_ll,&core_new,sizeof(XLcore));
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
    /*if(net==NULL||name==NULL)return -1;
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
    return  id;*/
    if(net==NULL||name==NULL)return -1;
    XLcore core_new;
    core_new.net=*net;
    core_new.safety=CORE_STATE_UNVERIFIED;
    core_new.sign_list=NULL;
    strcpy(core_new.name,name);
    core_new.id=1;

    extern XLll * core_ll;
    if(core_ll==NULL)core_ll=ll_create(sizeof(XLcore));
    //添加首个成员
    if(core_ll->head==NULL){
        ll_add_member_head(core_ll,&core_new,sizeof(XLcore));
        return 1;
    }
    //添加成员
    XLll_member * member_now=core_ll->head;
    int mode=0,mem_front_id=-1;//-1代表位于首位
    //获得新的成员的ID和要插入的位置
    for(int i=0;i<core_ll->member_num;i++){
        XLcore * core_now=(XLcore*)member_now->data;
        if(mode==0&&core_new.id==core_now->id){
            core_new.id++;
            mem_front_id=i;
        }
        else if(mode==1&&core_new.id==core_now->id){
            core_new.id++;
            mem_front_id=i;
            mode=0;
        }
        else if(mode==0)mode=1;

        if(core_now->net.ip==net->ip)return -1;
        member_now=member_now->next;
    }

    if(mem_front_id==-1)ll_insert_member_front(core_ll,&core_new,sizeof(XLcore),0);
    else ll_insert_member_next(core_ll,&core_new,sizeof(XLcore),mem_front_id);
    return core_new.id;

}

void core_show(void)
{
    printf("core show:\n");
    extern XLll * core_ll;
    if(core_ll==NULL)return;
    XLll_member * member_now=core_ll->head;
    if(member_now==NULL)return;
    for(int i=0;i<core_ll->member_num;i++)
    {
        XLcore * core_now=(XLcore*)member_now->data;
        printf("%d ",core_now->id);
        member_now=member_now->next;
    }
    printf("\n");
}

int core_remove(core_id_t id)
{
    extern XLll * core_ll;
    if(core_ll==NULL)return -1;
    XLll_member * member_now=core_ll->head;
    for(int i=0;i<core_ll->member_num;i++){
        XLcore * core_now=(XLcore*)member_now->data;
        if(core_now->id==id){
            //Warn:free_sign_list
            ll_del_member(core_ll,i);
            return 1;
        }
        member_now=member_now->next;
    }
    return -1;
}

XLcore * core_get_by_id(core_id_t id)
{
    extern XLll * core_ll;
    if(core_ll==NULL)return NULL;
    XLll_member * member_now=core_ll->head;
    for(int i=0;i<core_ll->member_num;i++){
        XLcore * core_now=(XLcore*)member_now->data;
        if(core_now->id==id)return core_now;
        member_now=member_now->next;
    }
    return NULL;
}

XLcore * core_get_by_net(XLnet * net)
{
    if(net==NULL)return NULL;
    extern XLll * core_ll;
    if(core_ll==NULL)return NULL;
    XLll_member * member_now=core_ll->head;
    for(int i=0;i<core_ll->member_num;i++){
        XLcore * core_now=(XLcore*)member_now->data;
        if(core_now->net.ip==net->ip)return core_now;
        member_now=member_now->next;
    }
    return NULL;
}

XLcore * core_get_by_ip(IP ip){
    extern XLll * core_ll;
    if(core_ll==NULL)return NULL;
    XLll_member * member_now=core_ll->head;
    for(int i=0;i<core_ll->member_num;i++){
        XLcore * core_now=(XLcore*)member_now->data;
        if(core_now->net.ip==ip)return core_now;
        member_now=member_now->next;
    }
    return NULL;
}

XLcore * core_get_by_name(char * name)
{
    if(name==NULL)return NULL;
    extern XLll * core_ll;
    if(core_ll==NULL)return NULL;
    XLll_member * member_now=core_ll->head;
    for(int i=0;i<core_ll->member_num;i++){
        XLcore * core_now=(XLcore*)member_now->data;
        if(strcmp(core_now->name,name))return core_now;
        member_now=member_now->next;
    }
    return NULL;
}

int core_rename(core_id_t id,char * name)
{
    if(name==NULL)return -1;
    XLcore * core=core_get_by_id(id);
    strcpy(core->name,name);
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
/*
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
}*/
