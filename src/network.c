#include<network.h>
XLins_queue * send_queue_head;
XLins_queue * recv_queue_head;

XLnet * network_get_local_info(void)
{
    XLnet * net_info=malloc(sizeof(net_info));
    net_info->ip=inet_addr("192.168.1.7");
    //net_info->mac=;
    net_info->port=8081;
    return  net_info;
}

/*XLins * queue_get(int queue)
{
    if(queue==SEND_QUEUE)
    {
        extern XLins_queue * send_queue_seek;
        return send_queue_seek->ins;
    }
    if(queue==RECIVE_QUEUE)
    {
        extern XLins_queue * recv_queue_seek;
        return recv_queue_seek->ins;
    }
    else return 0;
}

XLins * queue_del(int queue)
{
    XLins_queue * queue_head,*queue_seek;
    if(queue==SEND_QUEUE)
    {
        extern XLins_queue * send_queue_head;
        extern XLins_queue * send_queue_seek;
        queue_head=send_queue_head;
        queue_seek=send_queue_seek;
    }
    if(queue==RECIVE_QUEUE)
    {
        extern XLins_queue * recv_queue_head;
        extern XLins_queue * recv_queue_seek;
        queue_head=recv_queue_head;
        queue_seek=recv_queue_seek;
    }
    else return 0;

    if(queue_head==NULL)return 0;

}


int queue_add(int queue,XLins * ins,LEVEL level)
{
    XLins_queue * queue_head,*queue_seek;
    if(queue==SEND_QUEUE)
    {
        extern XLins_queue * send_queue_head;
        extern XLins_queue * send_queue_seek;
        queue_head=send_queue_head;
        queue_seek=send_queue_seek;
    }
    if(queue==RECIVE_QUEUE)
    {
        extern XLins_queue * recv_queue_head;
        extern XLins_queue * recv_queue_seek;
        queue_head=recv_queue_head;
        queue_seek=recv_queue_seek;
    }
    else return 0;
    printf("aa\n");
    if(queue_head==NULL)
    {
        queue_head=malloc(sizeof (XLins_queue));
        queue_head->ins=ins;
        queue_head->level=level;
        queue_head->front=queue_head;
        queue_head->next=queue_head;
        queue_seek=queue_head;
        return 1;
    }
    XLins_queue * ins_now=queue_head;
    while (1) {
        if(level>ins_now->level){
            XLins_queue * ins_new=malloc(sizeof (XLins_queue));
            ins_new->next=ins_now;
            ins_new->front=ins_now->front;

            ins_now->front->next=ins_new;
            ins_now->front=ins_new;

            ins_new->level=level;
            queue_seek=ins_new;
            if(ins_now==queue_head)queue_head=ins_new;
            return 1;
        }
        if(ins_now->next==queue_head)
        {
            XLins_queue * ins_new=malloc(sizeof (XLins_queue));
            ins_new->next=ins_now->next;
            ins_new->front=ins_now;

            ins_now->next->front=ins_new;
            ins_now->next=ins_new;

            ins_new->level=level;

            return 1;
        }
        ins_now=ins_now->next;
    }
    return 0;
}



void show_send_queue(int queue){
    XLins_queue * queue_head,*queue_seek;
    if(queue==SEND_QUEUE)
    {
        extern XLins_queue * send_queue_head;
        extern XLins_queue * send_queue_seek;
        queue_head=send_queue_head;
        queue_seek=send_queue_seek;
    }
    if(queue==RECIVE_QUEUE)
    {
        extern XLins_queue * recv_queue_head;
        extern XLins_queue * recv_queue_seek;
        queue_head=recv_queue_head;
        queue_seek=recv_queue_seek;
    }
    else return;
    if(queue_head==NULL)
    {
        printf("no!\n");
        return;
    }
    XLins_queue * ins_now=queue_head;
    while (1) {
        printf("%d ",ins_now->level);
        if(ins_now->next==queue_head)break;
        ins_now=ins_now->next;
    }
    printf("\nseek is%d:\n",queue_seek->level);
}
*/

int send_queue_del(void)
{
    extern XLins_queue * send_queue_head;
    if(send_queue_head->next==send_queue_head){
        free(send_queue_head);
        return 1;
    }
    send_queue_head->next->front=send_queue_head->front;
    send_queue_head->front->next=send_queue_head->next;
    XLins_queue * tmp=send_queue_head->next;
    free(send_queue_head);
    send_queue_head=tmp;
    return 1;
}

int send_queue_add(XLins * ins,LEVEL level)
{
    extern XLins_queue * send_queue_head;
    if(send_queue_head==NULL)
    {
        send_queue_head=malloc(sizeof (XLins_queue));
        send_queue_head->ins=ins;
        send_queue_head->level=level;
        send_queue_head->front=send_queue_head;
        send_queue_head->next=send_queue_head;
        return 1;
    }
    XLins_queue * ins_now=send_queue_head;
    while (1) {
        if(level>ins_now->level){
            XLins_queue * ins_new=malloc(sizeof (XLins_queue));
            ins_new->next=ins_now;
            ins_new->front=ins_now->front;

            ins_now->front->next=ins_new;
            ins_now->front=ins_new;

            ins_new->level=level;
            if(ins_now==send_queue_head)send_queue_head=ins_new;

            return 1;
        }
        if(ins_now->next==send_queue_head)
        {
            XLins_queue * ins_new=malloc(sizeof (XLins_queue));
            ins_new->next=ins_now->next;
            ins_new->front=ins_now;

            ins_now->next->front=ins_new;
            ins_now->next=ins_new;

            ins_new->level=level;
            return 1;
        }
        ins_now=ins_now->next;
    }
    return 0;
}

void show_send_queue(void){
    extern XLins_queue * send_queue_head;
    if(send_queue_head==NULL)
    {
        printf("no!\n");
        return;
    }
    XLins_queue * ins_now=send_queue_head;
    while (1) {
        printf("%d ",ins_now->level);
        if(ins_now->next==send_queue_head)break;
        ins_now=ins_now->next;
    }
    printf("\n");
}
