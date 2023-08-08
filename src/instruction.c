#include<instruction.h>
#include <network.h>
#include <event.h>

XLmonitor_list * monitor_head;
//----------------------------------------------//
//              INS的编码与解码                   //
//----------------------------------------------//

char * get_ins(char * s,int * p,int maxsize)
{
    if(*p>=maxsize)return 0;
    char * s_p=s,*new=malloc(sizeof(char)*(maxsize-*p)),*new_p=new;
    memset(new,0,sizeof (char)*(maxsize-*p));
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
        strncat(ins_out,(char*)decoded_ins->argv[i],strlen(decoded_ins->argv[i])+1);
        i++;
    }
    return ins_out;
}

// 将指令转化为一个一个参数的字符串
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
    num=0;
    int size=0;
    for(int i=0;i<len;i++){
        if((*ins_p==' '|*ins_p=='\0')&&mode==1){
            decode->argv[num]=malloc(sizeof(char)*size+1);
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
    char * s_p;
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

//----------------------------------------------//
//               指令的处理转发                   //
//----------------------------------------------//
int ins_send_to_event(XLpak_ins * ins){
    int AC=0,SA_mon_id=0;   //同意标识符，AC=1

    //指令为启动指定APP
    if(ins->receiver.mode==START_APP)
    {

        XLapp * app=app_get_by_name(ins->receiver.name);
        if(app==NULL)return -1;
        event_id_t event_id=event_create(app->event);
        if(event_id<=0)return -1;
        SA_mon_id=event_get_by_id(event_id)->mon_id;
        printf("mo:%d\n",SA_mon_id);
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
        XLsource *source;
        source=&monitor_now->monitor->receiver;

        if(AC&&monitor_now->monitor->id==SA_mon_id)num++;   //启动应用的时候
        else if(source->mode==ins->receiver.mode){
            if((source->mode==EVENT_ID||source->mode==ACCESS)
                &&source->id==ins->receiver.id)num++;
            if(source->mode==SIGN_NAME&&source->id==ins->receiver.id)num++;
        }
        if(num==1){
            XLsource_list *list=&monitor_now->monitor->list->source;
            if(list==NULL)queue_add_ins(&monitor_now->monitor->queue_head,ins,0);
            while(list!=NULL){
                if(source_cmp(&list->source,&ins->sender)){
                    queue_add_ins(&monitor_now->monitor->queue_head,ins,0);
                    break;
                }
                list=list->next;
            }
        }
        if(monitor_now->next!=NULL)monitor_now=monitor_now->next;
        else break;
    }

    return 1;
}
//----------------------------------------------//
//            指令监视器的基本操作                 //
//----------------------------------------------//
mon_id_t monitor_create(XLsource_list * send_list,XLsource * receive)
{
    int mode=0;
    extern XLmonitor_list * monitor_head;
    //XLsource_list * send_list=send_list;
    //if(list==NULL)list=source_list_create();

    if(monitor_head==NULL){
        monitor_head=malloc(sizeof(XLmonitor_list));
        monitor_head->monitor=malloc(sizeof(XLmonitor));
        monitor_head->monitor->id=1;
        monitor_head->monitor->list=send_list;
        monitor_head->monitor->receiver=*receive;
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
    monitor_new->monitor->list=send_list;
    monitor_new->monitor->receiver=*receive;
    monitor_new->monitor->queue_head.queue=NULL;

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

XLsource *monitor_get_source(mon_id_t id){
    XLmonitor * monitor=monitor_get_by_id(id);
    if(monitor!=NULL)return &monitor->receiver;
    return NULL;
}
XLpak_ins * monitor_get_member(mon_id_t monitor_id){
    XLmonitor * monitor=monitor_get_by_id(monitor_id);
    if(monitor==NULL)return NULL;
    if(monitor->queue_head.queue==NULL)return NULL;
    return &monitor->queue_head.queue->in.pak_ins;
}

INS * monitor_get_ins(mon_id_t monitor_id){
    XLmonitor * monitor=monitor_get_by_id(monitor_id);
    if(monitor==NULL)return NULL;
    if(monitor->queue_head.queue==NULL)return NULL;
    return &monitor->queue_head.queue->in.pak_ins.ins;
}

int monitor_remove_member(mon_id_t monitor_id){
    XLmonitor * monitor=monitor_get_by_id(monitor_id);
    if(monitor==NULL)return -1;
    if(monitor->queue_head.queue==NULL)return -1;
    return queue_del_head(&monitor->queue_head);
}

int monitor_remove_all_member(mon_id_t monitor_id){
    XLmonitor * monitor=monitor_get_by_id(monitor_id);
    if(monitor==NULL)return -1;
    queue_remove_all(&monitor->queue_head);
    return 1;
}

int monitor_limit_add_source(mon_id_t mon_id,XLsource * source){
    XLmonitor * monitor=monitor_get_by_id(mon_id);
    if(monitor==NULL)return -1;

    if(source==NULL){
        monitor->list=NULL;
        return 2;
    }

    XLsource_list * list=malloc(sizeof(XLsource_list));
    list->next=NULL;
    list->source=*source;
    if(monitor->list==NULL){
        monitor->list=list;
        return 1;
    }
    XLsource_list * list_now=monitor->list;
    while(list_now!=NULL){
        if(list_now->next==NULL){
            list_now->next=list;
            return 1;
        }
        list_now=list_now->next;
    }
    return -1;
}

//----------------------------------------------//
//            对指令的参数进行提取                 //
//----------------------------------------------//
int ins_get_par(INS * ins,char *name){
    XLins_decoded * decode=ins_decode(ins);
    for(int i=0;i<decode->argc;i++){
        if(strcmp(name,decode->argv[i])==0){free(decode);return 1;}
    }
    return 0;
}


char * ins_get_par_str(INS * ins,char *name){
    XLins_decoded * decode=ins_decode(ins);
    for(int i=0;i<decode->argc;i++){
        if(strcmp(name,decode->argv[i])==0&&i+1<=decode->argc)
            return (char*)decode->argv[i+1];
    }
    free(decode);
    return NULL;
}
int ins_get_par_int(INS *ins,char* name,int * val){
    char * str=ins_get_par_str(ins,name);
    if(str==NULL)return -1;
    *val=atoi(str);
    return 1;
}

int ins_get_par_ip(INS * ins,char *name,IP * ip){
    char * str=ins_get_par_str(ins,name);
    if(str==NULL)return -1;
    *ip=inet_addr(str);
    return 1;
}

//192.168.0.100:666:EID:6
int ins_get_par_source(INS *ins,char *name,XLsource * source){
    char * par=ins_get_par_str(ins,name);
    if(par==NULL)return -1;
    char * str[4],*str_p,*par_p=par;
    for(int i=0;i<4;i++)str[i]=malloc(strlen(ins)+1);
    str_p=str[0];
    int mode=1;
    int j;
    for( j=0;j<strlen(ins)+1;j++){
        if(*str_p=='\0')
        {
            if(mode!=4)break;
            else{
                XLsource new_source;
                new_source.net.ip=inet_addr(str[0]);
                new_source.net.port=atoi(str[1]);

                if(strcmp(str[2],"EID")==0){
                    new_source.mode=EVENT_ID;
                    new_source.id=atoi(str[3]);
                }
                else if(strcmp(str[2],"SN")==0){
                    new_source.mode=SIGN_NAME;
                    new_source.name=malloc(strlen(str[3])+1);
                    strcpy(new_source.name,str[3]);
                }
                for(int i=0;i<4;i++)free(str[i]);
                *source=new_source;
                printf("SSSSSSSSSSSS\n");
                return 1;
            }
        }
        else if(*str_p==':'){
            printf("here1\n");
            *str_p='\0';
            if(mode<4)mode++;
            else break;
            str_p=str[mode];
        }
        else{
            printf("%d",j);
            *str_p=*par_p;
            str_p++;
        }
        par_p++;
    }
    for(int i=0;i<4;i++)free(str[i]);
    printf("mode:%d j:%d\n",mode,j);
    return -2;
}



char * ins_source_to_str(XLsource *source){
    if(source==NULL)return NULL;
    char str[40],mode[10];
    if(source->mode==EVENT_ID)strcpy(mode,"EID");
    else if(source->mode==SIGN_NAME)strcpy(mode,"SN");
    sprintf(str,"%s:%d:%s:%d",inet_addr(source->net.ip),source->net.port,mode,source->id);
    return str;
}

int source_cmp(XLsource *source1,XLsource *source2){
    int i=0;
    if(source1->mode==source2->mode)i++;
    if((source1->mode==EVENT_ID||source1->mode==ACCESS)&&source1->id==source2->id)i+=1;
    if(source1->mode==SIGN_NAME&&strcmp(source1->name,source2->name)==0)i+=1;
    if(source1->net.ip==source2->net.ip)i+=1;
    printf("i:%d\n",i);
    if(i==3)return 1;
    return 0;
}
