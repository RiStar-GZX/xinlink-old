#include <event.h>

XLevent_list * event_list_head;

int event_add(str * name,EVENT event)
{
    event_id_t id=1;
    int mode=0;
    extern XLevent_list * event_list_head;

    if(event_list_head==NULL)
    {
        event_list_head=malloc(sizeof (struct XLevent_list));
        event_list_head->event=malloc(sizeof (XLevent));
        event_list_head->event->id=1;
        event_list_head->event->event=event;
        strcpy(event_list_head->event->name,name);
        event_list_head->next=NULL;
        return 1;
    }

    XLevent_list * event_now=event_list_head,*event_front=NULL;

    while (1)
    {
        if(strcmp(name,event_now->event->name)==0)return -2;
        if(event_now->next==NULL)break;
        event_now=event_now->next;
    }

    event_now=event_list_head;
    while (1)
    {
        if(id==event_now->event->id)id++;
        else if(id<event_now->event->id)
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
    event_new->event=malloc(sizeof (XLevent));
    strcpy(event_new->event->name,name);
    event_new->event->id=id;
    event_new->event->event=event;

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

void event_list(void)
{
    printf("event list:");
    extern XLevent_list * event_list_head;
    XLevent_list * event_now=event_list_head;
    if(event_now==NULL)
    {
        printf("no event!\n");
        return;
    }

    while (1) {
        printf("%d ",event_now->event->id);
        if(event_now->next==NULL)break;
        event_now=event_now->next;
    }

    printf("\n");
}

int event_remove(event_id_t id)
{
    extern XLevent_list * event_list_head;
    XLevent_list * event_now=event_list_head,*event_front=event_now;
    if(event_now==NULL)return -2;
    while (1) {
        if(id==event_now->event->id)
        {
            if(event_now==event_list_head)
                event_list_head=event_now->next;
            else
                event_front->next=event_now->next;
            free(event_now->event);
            return 1;
        }
        if(event_now->next==NULL)break;
        event_front=event_now;
        event_now=event_now->next;
    }
    printf("no!\n");
    return  -2;
}


XLevent * event_get_by_id(event_id_t id)
{
    extern XLevent_list * event_list_head;
    XLevent_list * event_now=event_list_head;
    if(event_now==NULL)return NULL;
    while (1) {
        if(id==event_now->event->id)return event_now->event;
        if(event_now->next==NULL)return NULL;
        event_now=event_now->next;
    }
    return NULL;
}

XLevent * event_get_by_name(str * name)
{
    extern XLevent_list * event_list_head;
    XLevent_list * event_now=event_list_head;
    if(event_now==NULL)return NULL;
    while (1) {
        if(strcmp(name,event_now->event->name)==0)return event_now->event;
        if(event_now->next==NULL)return NULL;
        event_now=event_now->next;
    }
    return NULL;
}

int event_set(str * name,EVENT event)
{
    XLevent * event_get;
    event_get=event_get_by_name(name);
    if(event_get==NULL)return -1;
    event_get->event=event;
    return 1;
}

INS * event(str *name,INS * ins)
{
    XLevent * event;
    event=event_get_by_name(name);
    if(event==NULL)return 0;
    return event->event(ins);
}

