#include<instruction.h>
#include <network.h>
#include <event.h>

XLmonitor_list * monitor_head;
//   将解码后的ins转化为字符形式
INS * ins_encode(XLins_decoded * decoded_ins)
{
    //Error
    if(decoded_ins==NULL)return 0;
    //Measurement data size
    int ins_size=1,i=0;
    //XLins_decoded_data
    while (decoded_ins->argc!=i) {
        if(decoded_ins->argv[i]==NULL)return NULL;
        ins_size+=strlen(decoded_ins->argv[i])+2;
        i++;
    }
    printf("size:%d\n",ins_size);

    //Encode uncoded data
    INS * ins_out=malloc(sizeof (INS)*ins_size);
    memset(ins_out,0,sizeof (INS)*ins_size);
    i=0;
    while (decoded_ins->argc!=i) {
        strcat(ins_out," ");
        strncat(ins_out,(str*)decoded_ins->argv[i],strlen(decoded_ins->argv[i])+1);
        i++;
    }
    return ins_out;
}

str * get_ins(str * s,int * p,int maxsize)
{
    if(*p>=maxsize)return 0;
    str * s_p=s,*new=malloc(sizeof(str)*(maxsize-*p)),*new_p=new;
    memset(new,0,sizeof (str)*(maxsize-*p));
    int mode=0,i=0;
    s_p+=*p;
    while (1) {
        if(*s_p==' '&&mode==0){ //skip space
            s_p++;
            i++;
        }
        else if((*s_p==' '||*s_p=='\0')&&mode==1)   //end of str
        {
            i++;
            *p+=i;
            break;
        }
        else {
            *new_p=*s_p;
            new_p++;
            s_p++;
            i++;
            mode=1;
        }
        if(*p>maxsize)return 0;
    }
    //write endl
    new_p++;
    *new_p='\0';
    return new;
}

//  字符形式的ins转化为参数形式
XLins_decoded * ins_decode(INS * ins)
{
    if(ins==NULL)return 0;

    XLins_decoded * decode=malloc(sizeof (XLins_decoded));
    memset(decode,0,sizeof (XLins_decoded));

    //读取字符串中的参数个数
    INS * ins_p=ins;
    int len=strlen(ins)+1,num=0,mode=0;
    for(int i=0;i<len;i++){
        if((*ins_p==' '|*ins_p=='\0')&&mode==1){mode=0;num++;}
        else if(*ins_p!=' '&&*ins_p!='\0'&&mode==0)mode=1;
        ins_p++;
    }

    //根据刚才读出的参数个数，创建字符串指针
    decode->argv=malloc(sizeof(uint8_t*)*num);

    //为各个字符串设置内存空间
    ins_p=ins;
    mode=0;
    decode->argc=num;
    printf("num:%d\n",num);
    num=0;
    int size=0;
    for(int i=0;i<len;i++){
        if((*ins_p==' '|*ins_p=='\0')&&mode==1){
            decode->argv[num]=malloc(sizeof(str)*size+1);
            mode=0;
            size=0;
            num++;
        }
        else if(*ins_p!=' '&&*ins_p!='\0'&&mode==0){size=0;mode=1;}
        ins_p++;
        size++;
    }

    //将字符串中参数单独复制对应字符串中
    ins_p=ins;
    mode=0;
    num=0;
    str * s_p;
    for(int i=0;i<len;i++){
        if((*ins_p==' '|*ins_p=='\0')&&mode==1){
            *s_p='\0';
            mode=0;
            num++;
        }
        else if(*ins_p!=' '&&*ins_p!='\0'&&mode==0){
            s_p=decode->argv[num];
            mode=1;
        }
        if(*ins_p!=' '&&*ins_p!='\0'&&mode==1){
            *s_p=*ins_p;
            s_p++;
        }
        ins_p++;
    }
    return decode;
}

//send ins to (start) event
int ins_send_to_event(XLpak_ins * ins){
    int AC=0,SA_mon_id=0;   //同意标识符，AC=1

    //指令为启动指定APP
    if(ins->receiver.mode==RECEIVER_START_APP)
    {
        XLapp * app=app_get_by_name(ins->receiver.name);
        if(app==NULL)return -1;
        event_id_t event_id=event_create(app->event);
        if(event_id<=0)return -1;
        SA_mon_id=event_get_by_id(event_id)->event.mon_id;
        //运行事件(会在另一个线程中运行事件）
        event_run(event_id);
        AC=1;
    }
    //添加指令到各个监视器中
    extern XLmonitor_list * monitor_head;
    XLmonitor_list * monitor_now=monitor_head;
    while(monitor_now!=NULL&&monitor_now->monitor!=NULL)
    {
        int num=0;
        XLsource * sender,*receiver;
        sender=&monitor_now->monitor->sender;
        receiver=&monitor_now->monitor->receiver;

        if(sender->mode==MONITOR_MODE_NONE)num++;
        else if(sender->id==ins->sender.id&&sender->mode==ins->sender.mode)num++;

        if(AC&&monitor_now->monitor->id==SA_mon_id)num++;   //启动应用的时候
        else if(receiver->id==ins->receiver.id&&receiver->mode==ins->receiver.mode)num++;

        if(num>=2){
            queue_add_ins(&monitor_now->monitor->queue_head,ins,0);
        }
        if(monitor_now->next!=NULL)monitor_now=monitor_now->next;
        else break;
    }

    return 1;
}

mon_id_t monitor_create(XLsource * send,XLsource * receive)
{
    int mode=0;
    extern XLmonitor_list * monitor_head;
    XLsource sender,receiver;
    if(send==NULL)sender.mode=MONITOR_MODE_NONE;
    else sender=*send;
    if(receive==NULL)receiver.mode=MONITOR_MODE_NONE;
    else receiver=*receive;

    if(monitor_head==NULL){
        monitor_head=malloc(sizeof(XLmonitor_list));
        monitor_head->monitor=malloc(sizeof(XLmonitor));
        monitor_head->monitor->id=1;
        monitor_head->monitor->sender=sender;
        monitor_head->monitor->receiver=receiver;
        monitor_head->next=NULL;
        return monitor_head->monitor->id;
    }

    XLmonitor_list * monitor_now=monitor_head,*monitor_front=NULL;
    mon_id_t id=1;
    while (1)
    {
        if(id==monitor_now->monitor->id)id++;
        else if(id<monitor_now->monitor->id)
        {
            if(monitor_front==NULL)mode=1;
            else mode=2;
            break;
        }
        else break;
        if(monitor_now->next==NULL)break;
        monitor_front=monitor_now;
        monitor_now=monitor_now->next;
    }

    XLmonitor_list * monitor_new=malloc(sizeof (XLmonitor_list));
    monitor_new->monitor=malloc(sizeof (XLmonitor));
    monitor_new->monitor->id=id;
    monitor_new->monitor->sender=sender;
    monitor_new->monitor->receiver=receiver;
    monitor_new->monitor->queue_head.queue==NULL;

    if(mode==1)
    {
        monitor_new->next=monitor_head;
        monitor_head=monitor_new;
    }
    else if(mode==2)
    {
        monitor_front->next=monitor_new;
        monitor_new->next=monitor_now;
    }
    else
    {
        monitor_now->next=monitor_new;
        monitor_new->next=NULL;
    }
    return id;
}

void monitor_list(void)
{
    printf("monitor list:");
    extern XLmonitor_list * monitor_head;
    XLmonitor_list * monitor_now=monitor_head;
    if(monitor_now==NULL)
    {
        printf("no monitor!\n");
        return;
    }

    while (1) {
        printf("%d ",monitor_now->monitor->id);
        if(monitor_now->next==NULL)break;
        monitor_now=monitor_now->next;
    }

    printf("\n");
}

int monitor_remove(mon_id_t id)
{
    extern XLmonitor_list * monitor_head;
    XLmonitor_list * monitor_now=monitor_head,*monitor_front=monitor_now;
    if(monitor_now==NULL)return -2;
    while (1) {
        if(id==monitor_now->monitor->id)
        {
            if(monitor_now==monitor_head)
                monitor_head=monitor_now->next;
            else
                monitor_front->next=monitor_now->next;
            free(monitor_now->monitor);
            free(monitor_now);
            return 1;
        }
        if(monitor_now->next==NULL)break;
        monitor_front=monitor_now;
        monitor_now=monitor_now->next;
    }
    printf("no!\n");
    return  -2;
}

XLmonitor * monitor_get_by_id(mon_id_t monitor_id){
    extern XLmonitor_list * monitor_head;
    XLmonitor_list * list_now=monitor_head;
    while(list_now!=NULL)
    {
        if(list_now->monitor!=NULL)
        {
            if(list_now->monitor->id==monitor_id)return list_now->monitor;
        }
        if(list_now->next==NULL)return NULL;
        list_now=list_now->next;
    }
    return NULL;
}

XLpak_ins * monitor_get_member(mon_id_t monitor_id){
    XLmonitor * monitor=monitor_get_by_id(monitor_id);
    if(monitor==NULL)return NULL;
    if(monitor->queue_head.queue==NULL)return NULL;
    return &monitor->queue_head.queue->in.pak_ins;
}

int monitor_remove_member(mon_id_t monitor_id){
    XLmonitor * monitor=monitor_get_by_id(monitor_id);
    if(monitor==NULL)return -1;
    if(monitor->queue_head.queue==NULL)return -1;
    return queue_del(&monitor->queue_head);
}


