#include<device.h>

XLdev_list * device_list_head;

int device_add(str * name)
{
    dev_id_t id=1;
    int mode=0;
    extern XLdev_list * device_list_head;

    if(device_list_head==NULL)
    {
        device_list_head=malloc(sizeof (struct XLdev_list));
        device_list_head->dev=malloc(sizeof (XLdev));
        device_list_head->dev->id=1;
        strcpy(device_list_head->dev->name,name);
        device_list_head->next=NULL;
        return 1;
    }

    XLdev_list * device_now=device_list_head,*device_front=NULL;

    while (1)
    {
        if(strcmp(name,device_now->dev->name)==0)return -2;
        if(device_now->next==NULL)break;
        device_now=device_now->next;
    }

    device_now=device_list_head;
    while (1)
    {
        if(id==device_now->dev->id)id++;
        else if(id<device_now->dev->id)
        {
            if(device_front==NULL)mode=1;
            else mode=2;
            break;
        }
        else break;
        if(device_now->next==NULL)break;
        device_front=device_now;
        device_now=device_now->next;
    }

    XLdev_list * device_new=malloc(sizeof (XLdev_list));
    device_new->dev=malloc(sizeof (XLdev));
    strcpy(device_new->dev->name,name);
    device_new->dev->id=id;

    if(mode==1)
    {
        device_new->next=device_list_head;
        device_list_head=device_new;
    }
    else if(mode==2)
    {
        device_front->next=device_new;
        device_new->next=device_now;
    }
    else
    {
        device_now->next=device_new;
        device_new->next=NULL;
    }
    return id;
}

void device_list(void)
{
    printf("device list:");
    extern XLdev_list * device_list_head;
    XLdev_list * device_now=device_list_head;
    if(device_now==NULL)
    {
        printf("no device!\n");
        return;
    }

    while (1) {
        printf("%d ",device_now->dev->id);
        if(device_now->next==NULL)break;
        device_now=device_now->next;
    }

    printf("\n");
}

int device_remove(dev_id_t id)
{
    extern XLdev_list * device_list_head;
    XLdev_list * device_now=device_list_head,*device_front=device_now;
    if(device_now==NULL)return -2;
    while (1) {
        if(id==device_now->dev->id)
        {
            if(device_now==device_list_head)
                device_list_head=device_now->next;
            else
                device_front->next=device_now->next;
            free(device_now->dev);
            return 1;
        }
        if(device_now->next==NULL)break;
        device_front=device_now;
        device_now=device_now->next;
    }
    printf("no!\n");
    return  -2;
}


XLdev * device_get_by_id(dev_id_t id)
{
    extern XLdev_list * device_list_head;
    XLdev_list * device_now=device_list_head;
    if(device_now==NULL)return NULL;
    while (1) {
        if(id==device_now->dev->id)return device_now->dev;
        if(device_now->next==NULL)return NULL;
        device_now=device_now->next;
    }
    return NULL;
}

XLdev * device_get_by_name(str * name)
{
    extern XLdev_list * device_list_head;
    XLdev_list * device_now=device_list_head;
    if(device_now==NULL)return NULL;
    while (1) {
        if(strcmp(name,device_now->dev->name)==0)return device_now->dev;
        if(device_now->next==NULL)return NULL;
        device_now=device_now->next;
    }
    return NULL;
}

