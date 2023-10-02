#include <core.h>

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
    core_new.sign_ll=ll_create(sizeof(XLsign));
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

int core_add(XLnet * net,const char* name)
{
    if(net==NULL||name==NULL)return -1;
    XLcore core_new;
    core_new.net=*net;
    core_new.safety=CORE_STATE_UNVERIFIED;
    core_new.sign_ll=ll_create(sizeof(XLsign));
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
            //Warn:free_sign_ll
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

XLcore * core_get_by_name(const char* name)
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

int core_rename(core_id_t id,const char* name)
{
    if(name==NULL)return -1;
    XLcore * core=core_get_by_id(id);
    strcpy(core->name,name);
    return 1;
}

int core_add_sign(core_id_t core_id,XLll * sign_ll,int mode){
    if(sign_ll==NULL||core_id!=CORE_MYSELF_ID||sign_ll->head==NULL)return -1;
    XLcore * core=core_get_by_id(core_id);
    if(core==NULL)return -1;
    if(core->sign_ll==NULL||core->sign_ll->head==NULL)return -1;
    if(core->sign_ll->head!=NULL&&mode==0){
        ll_del_member_all(core->sign_ll);
        core->sign_ll=sign_ll;
    }
    else if(mode==1){
        ll_add_ll(core->sign_ll,sign_ll);
    }
    return 1;
}

int core_sign_remove_all(core_id_t core_id){
    if(core_id==CORE_MYSELF_ID)return -1;
    XLcore * core=core_get_by_id(core_id);
    if(core==NULL)return -1;
    ll_del_member_all(core->sign_ll);
    return 1;
}

XLll *  core_get_sign_ll(core_id_t core_id){
    if(core_id==CORE_MYSELF_ID){
        extern XLll* event_ll;
        if(event_ll==NULL||event_ll->head==NULL)return NULL;
        XLll * sign_ll=ll_create(sizeof(XLsign));
        XLll_member * member=event_ll->head;
        for(int i=0;i<event_ll->member_num;i++){
            XLevent * event=(XLevent*)member->data;
            if(event->sign.use==ENABLE){
                ll_add_member_tail(sign_ll,&event->sign,sizeof(XLsign));
                printf("name:%s\n",event->sign.name);
            }
            member=member->next;
        }
        return sign_ll;
    }
    XLcore * core =core_get_by_id(core_id);
    if(core==NULL)return NULL;
    return core->sign_ll;
}
