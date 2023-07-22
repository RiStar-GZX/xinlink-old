#include <event.h>
#include <network.h>

XLapp_list * app_list_head;
XLevent_list * event_list_head;

int app_add(str * name,EVENT event)
{
    app_id_t id=1;
    int mode=0;
    extern XLapp_list * app_list_head;

    if(app_list_head==NULL)
    {
        app_list_head=malloc(sizeof (struct XLapp_list));
        app_list_head->app=malloc(sizeof (XLapp));
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

    XLapp_list * app_new=malloc(sizeof (XLapp_list));
    app_new->app=malloc(sizeof (XLapp));
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
    XLapp_list * event_now=app_list_head,*event_front=event_now;
    if(event_now==NULL)return -2;
    while (1) {
        if(id==event_now->app->id)
        {
            if(event_now==app_list_head)
                app_list_head=event_now->next;
            else
                event_front->next=event_now->next;
            free(event_now->app);
            return 1;
        }
        if(event_now->next==NULL)break;
        event_front=event_now;
        event_now=event_now->next;
    }
    printf("no!\n");
    return  -2;
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

XLapp * app_get_by_name(str * name)
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

int app_set(str * name,EVENT event)
{
    XLapp * app_get;
    app_get=app_get_by_name(name);
    if(app_get==NULL)return -1;
    app_get->event=event;
    return 1;
}



event_id_t event_create(EVENT EVENT){
    if(EVENT==NULL)return -1;
    extern XLevent_list * event_list_head;
    XLsource source;
    source.mode=RECEIVER_EVENT_ID;
    source.name=NULL;

    if(event_list_head==NULL)
    {
        event_list_head=malloc(sizeof (XLevent_list));
        memset(event_list_head,0,sizeof (XLevent_list));
        event_list_head->event.event=EVENT;
        event_list_head->event.id=1;
        source.id=1;
        event_list_head->event.mon_id=monitor_create(NULL,&source);

        return 1;
    }

    XLevent_list * event_now=event_list_head,*event_front=NULL;
    int mode=0,id=1;
    event_now=event_list_head;
    while (1)
    {
        if(id==(int)event_now->event.id)id++;
        else if(id<(int)event_now->event.id)
        {
            if(event_front==NULL)mode=1;
            else mode=2;
            break;
        }
        else break;
        if(event_now->next==NULL)break;
        event_front=event_now;
        event_now=event_now->next;
    }

    XLevent_list * event_new=malloc(sizeof (XLevent_list));
    memset(event_new,0,sizeof (XLevent_list));
    event_new->event.id=id;
    event_new->event.event=EVENT;
    source.id=id;
    event_new->event.mon_id=monitor_create(NULL,&source);

    if(mode==1)
    {
        event_new->next=event_list_head;
        event_list_head=event_new;
    }
    else if(mode==2)
    {
        event_front->next=event_new;
        event_new->next=event_now;
    }
    else
    {
        event_now->next=event_new;
        event_new->next=NULL;
    }
    return id;
}

XLevent_list * event_get_by_id(event_id_t id)
{
    extern XLevent_list * event_list_head;
    XLevent_list * event_now=event_list_head;
    if(event_now==NULL)return NULL;
    while (1) {
        if(id==event_now->event.id)return event_now;
        if(event_now->next==NULL)return NULL;
        event_now=event_now->next;
    }
    return NULL;
}

void * event_thread(void * arg)
{
    XLevent_list *event_arg=(XLevent_list *)arg;
    event_arg->event.event(event_arg->event.mon_id);
    return NULL;
}

int event_run(event_id_t id)
{
    pthread_t thread;
    XLevent_list * event_list=event_get_by_id(id);
    if(event_list==NULL)return -1;
    if(event_list->event.event==NULL)return -1;

    pthread_create(&thread,NULL,event_thread,event_list);
    if(!thread)perror("thread");
    return 1;
}

void event_show(void){
    extern XLevent_list * event_list_head;
    XLevent_list * event_now=event_list_head;
    if(event_now==NULL)return;
    printf("event list:");
    while(1)
    {
        printf("%d ",event_now->event.id);
        if(event_now->next==NULL)break;
        event_now=event_now->next;
    }
    printf("\n");
}
