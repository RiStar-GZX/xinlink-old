#include <event.h>
#include <network.h>

XLapp_list * app_list_head;
XLll * event_ll;

int app_add(char * name,EVENT event)
{
    app_id_t id=1;
    int mode=0;
    extern XLapp_list * app_list_head;

    if(app_list_head==NULL)
    {
        app_list_head=(XLapp_list*)malloc(sizeof (struct XLapp_list));
        app_list_head->app=(XLapp*)malloc(sizeof (XLapp));
        app_list_head->app->id=1;
        app_list_head->app->event=event;
        strcpy(app_list_head->app->name,name);
        app_list_head->next=NULL;
        return 1;
    }

    XLapp_list * app_now=app_list_head,*app_front=NULL;

    while (1)
    {
        if(strcmp(name,app_now->app->name)==0)return -2;
        if(app_now->next==NULL)break;
        app_now=app_now->next;
    }

    app_now=app_list_head;
    while (1)
    {
        if(id==app_now->app->id)id++;
        else if(id<app_now->app->id)
        {
            if(app_front==NULL)mode=1;
            else mode=2;
            break;
        }
        else break;
        if(app_now->next==NULL)break;
        app_front=app_now;
        app_now=app_now->next;
    }

    XLapp_list * app_new=(XLapp_list*)malloc(sizeof (XLapp_list));
    app_new->app=(XLapp*)malloc(sizeof (XLapp));
    strcpy(app_new->app->name,name);
    app_new->app->id=id;
    app_new->app->event=event;

    if(mode==1)
    {
        app_new->next=app_list_head;
        app_list_head=app_new;
    }
    else if(mode==2)
    {
        app_front->next=app_new;
        app_new->next=app_now;
    }
    else
    {
        app_now->next=app_new;
        app_new->next=NULL;
    }
    return id;
}

void app_list(void)
{
    printf("app list:\n");
    extern XLapp_list * app_list_head;
    XLapp_list * event_now=app_list_head;
    if(event_now==NULL)
    {
        printf("no app!\n");
        return;
    }

    while (1) {
        printf("ID:%d NAME:%s\n",event_now->app->id,event_now->app->name);
        if(event_now->next==NULL)break;
        event_now=event_now->next;
    }
}

int app_remove(app_id_t id)
{
    extern XLapp_list * app_list_head;
    XLapp_list * app_now=app_list_head,*app_front=app_now;
    if(app_now==NULL)return -1;
    while (1) {
        if(id==app_now->app->id)
        {
            if(app_now==app_list_head)
                app_list_head=app_now->next;
            else
                app_front->next=app_now->next;
            free(app_now->app);
            return 1;
        }
        if(app_now->next==NULL)break;
        app_front=app_now;
        app_now=app_now->next;
    }
    return  -1;
}

XLapp * app_get_by_id(app_id_t id)
{
    extern XLapp_list * app_list_head;
    XLapp_list * event_now=app_list_head;
    if(event_now==NULL)return NULL;
    while (1) {
        if(id==event_now->app->id)return event_now->app;
        if(event_now->next==NULL)return NULL;
        event_now=event_now->next;
    }
    return NULL;
}

XLapp * app_get_by_name(char * name)
{
    extern XLapp_list * app_list_head;
    XLapp_list * app_now=app_list_head;
    if(app_now==NULL)return NULL;
    while (1) {
        if(strcmp(name,app_now->app->name)==0)return app_now->app;
        if(app_now->next==NULL)return NULL;
        app_now=app_now->next;
    }
    return NULL;
}

int app_set(char * name,EVENT event)
{
    XLapp * app_get;
    app_get=app_get_by_name(name);
    if(app_get==NULL)return -1;
    app_get->event=event;
    return 1;
}



event_id_t event_create(EVENT EVENT){
    if(EVENT==NULL)return -1;
    XLsource source;
    source.mode=EVENT_ID;
    source.name=NULL;
    XLevent event_new;
    event_new.event=EVENT;
    event_new.sign.use=DISABLE;
    event_new.id=1;

    extern XLll * event_ll;
    if(event_ll==NULL)event_ll=ll_create(sizeof(XLevent));
    //添加首个成员
    if(event_ll->head==NULL){
        source.id=event_new.id;
        event_new.mon_id=monitor_create(&source);
        ll_add_member_head(event_ll,&event_new,sizeof(XLevent));
        return 1;
    }
    //添加成员
    XLll_member * member_now=event_ll->head;
    int mode=0,mem_front_id=-1;//-1代表位于首位
    //获得新的成员的ID和要插入的位置
    for(int i=0;i<event_ll->member_num;i++){
        XLevent * event_now=(XLevent*)member_now->data;
        if(mode==0&&event_new.id==event_now->id){
            event_new.id++;
            mem_front_id=i;
        }
        else if(mode==1&&event_new.id==event_now->id){
            event_new.id++;
            mem_front_id=i;
            mode=0;
        }
        else if(mode==0)mode=1;

        member_now=member_now->next;
    }

    source.id=event_new.id;
    event_new.mon_id=monitor_create(&source);
    if(mem_front_id==-1)ll_insert_member_front(event_ll,&event_new,sizeof(XLevent),0);
    else ll_insert_member_next(event_ll,&event_new,sizeof(XLevent),mem_front_id);
    return source.id;
}

int event_remove(event_id_t id)
{
    extern XLll * event_ll;
    if(event_ll==NULL)return -1;
    XLll_member * member_now=event_ll->head;
    for(int i=0;i<event_ll->member_num;i++){
        XLevent * event_now=(XLevent*)member_now->data;
        if(event_now->id==id){
            ll_del_member(event_ll,i);
            return 1;
        }
        member_now=member_now->next;
    }
    return -1;
}

XLevent * event_get_by_id(event_id_t id)
{
    extern XLll * event_ll;
    if(event_ll==NULL)return NULL;
    XLll_member * member_now=event_ll->head;
    for(int i=0;i<event_ll->member_num;i++){
        XLevent * event=(XLevent*)member_now->data;
        if(event->id==id)return event;
        member_now=member_now->next;
    }
    return NULL;
}

#ifdef PLATFORM_ESP32
void event_thread(void * arg)
#endif
#ifdef PLATFORM_LINUX
void * event_thread(void * arg)
#endif
{
    XLevent *event=(XLevent*)arg;
    XLevent_par par;
    par.id=event->id;
    par.mon_id=event->mon_id;
    par.sign=&event->sign;
    event->event(&par);
    event_remove(event->id);
#ifdef PLATFORM_LINUX
    return NULL;
#endif
}
#ifdef PLATFORM_LINUX
int event_run(event_id_t id)
{
    pthread_t thread;
    XLevent * event=event_get_by_id(id);
    if(event==NULL)return -1;
    pthread_create(&thread,NULL,event_thread,event);
    if(!thread)perror("thread");
    return 1;
}
#endif

#ifdef PLATFORM_ESP32
int event_run(event_id_t id)
{
    //pthread_t thread;
    XLevent * event=event_get_by_id(id);
    xTaskCreate(event_thread,"broadcast_recv",4096,event,0,NULL);
    return 1;
}
#endif
void event_show(void){
    extern XLll * event_ll;
    if(event_ll==NULL)return;
    XLll_member * member_now=event_ll->head;
    if(member_now==NULL)return;
    printf("event list:");
    for(int i=0;i<event_ll->member_num;i++)
    {
        XLevent * event_now=(XLevent*)member_now->data;
        printf("%d ",event_now->id);
        member_now=member_now->next;
    }
    printf("\n");
}

event_id_t event_create_and_run(char * app_name){
    XLapp * app=app_get_by_name(app_name);
    if(app==NULL)return -1;
    event_id_t event_id=event_create(app->event);
    if(event_id<=0)return -1;
    //运行事件(会在另一个线程中运行事件）
    event_run(event_id);
    return event_id;
}

int event_add_sign(event_id_t event_id,char * sign_name,char * type){
    if(sign_name==NULL&&type==NULL)return -1;
    //if(event_get_by_signname(sign_name)>0)return -2;
    XLevent * event=event_get_by_id(event_id);
    if(event==NULL)return -1;
    printf("here\n");
    event->sign.use=ENABLE;
    event->sign.name=(char*)malloc(sizeof(strlen(sign_name)+1));
    strcpy(event->sign.name,sign_name);
    event->sign.type=(char*)malloc(sizeof(strlen(type)+1));
    strcpy(event->sign.type,type);
    event->sign.contrast=NULL;
    event->sign.contrast_size=-1;
    return 1;
}

int event_remove_sign(event_id_t event_id){
    XLevent * event=event_get_by_id(event_id);
    if(event==NULL)return -1;
    if(event->sign.name!=NULL)free(event->sign.name);
    if(event->sign.contrast!=NULL)free(event->sign.contrast);
    if(event->sign.type!=NULL)free(event->sign.type);
    event->sign.use=DISABLE;
    return 1;
}


event_id_t event_get_by_signname(char * sign_name){
    if(sign_name==NULL)return -1;
    extern XLll * event_ll;
    if(event_ll==NULL)return -1;
    XLll_member * member_now=event_ll->head;
    for(int i=0;i<event_ll->member_num;i++){
        XLevent * event_now=(XLevent*)member_now->data;
        if(event_now->sign.use==ENABLE&&event_now->sign.name!=NULL){
            if(strcmp(event_now->sign.name,sign_name)==0)
            return event_now->id;
        }
        member_now=member_now->next;
    }
    return -1;
}

XLsign *event_get_sign(event_id_t event_id){
    XLevent * event=event_get_by_id(event_id);
    if(event==NULL)return NULL;
    return &event->sign;
}

