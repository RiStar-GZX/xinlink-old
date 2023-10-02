#include<instruction.h>
#include <network.h>
#include <event.h>

XLll * monitor_ll;
//----------------------------------------------//
//              INS的编码与解码                   //
//----------------------------------------------//

char * get_ins(char * s,int * p,int maxsize)
{
    if(*p>=maxsize)return 0;
    char * s_p=s,*ins=(char*)malloc(sizeof(char)*(maxsize-*p)),*ins_p=ins;
    memset(ins,0,sizeof (char)*(maxsize-*p));
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
            *ins_p=*s_p;
            ins_p++;
            s_p++;
            i++;
            mode=1;
        }
        if(*p>maxsize)return 0;
    }
    //write endl
    ins_p++;
    *ins_p='\0';
    return ins;
}

INS * ins_encode(XLins_decoded * decoded_ins)
{
    //Error
    if(decoded_ins==NULL)return 0;
    //Measurement data size
    int ins_size=1,i=0;
    //XLins_decoded_data
    while (decoded_ins->argc!=i) {
    //for(i=0;i<decoded_ins->argc;i++)
        if(decoded_ins->argv[i]==NULL)return NULL;
        ins_size+=strlen((char*)decoded_ins->argv[i])+2;
        i++;
    }

    printf("size:%d\n",ins_size);
    //Encode uncoded data
    INS * ins_out=(INS*)malloc(sizeof (INS)*ins_size);
    memset(ins_out,0,sizeof (INS)*ins_size);
    i=0;
    while (decoded_ins->argc!=i) {
        strcat(ins_out," ");
        strncat(ins_out,(char*)decoded_ins->argv[i],strlen((char*)decoded_ins->argv[i])+1);
        i++;
    }
    return ins_out;
}

// 将指令转化为一个一个参数的字符串
XLins_decoded * ins_decode(INS * ins)
{
    if(ins==NULL)return 0;

    XLins_decoded * decode=(XLins_decoded*)malloc(sizeof (XLins_decoded));
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
    //——————这里有问题
    decode->argv=(uint8_t**)malloc(sizeof(uint8_t*)*num);

    //为各个字符串设置内存空间
    ins_p=ins;
    mode=0;
    decode->argc=num;
    num=0;
    int size=0;
    for(int i=0;i<len;i++){
        if((*ins_p==' '|*ins_p=='\0')&&mode==1){
            decode->argv[num]=(uint8_t*)malloc(sizeof(char)*size+1);
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
    uint8_t * s_p;
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
        event_id_t event_id=event_create(app->init,app->event,app->exit);
        if(event_id<=0)return -1;
        SA_mon_id=event_get_by_id(event_id)->mon_id;
        printf("mo:%d\n",SA_mon_id);
        //运行事件(会在另一个线程中运行事件）
        event_run(event_id);
        AC=1;
    }
    //模式是设备标识符
    extern XLll * event_ll;
    if(ins->receiver.mode==SIGN_NAME&&ins->receiver.name!=NULL&&event_ll!=NULL){
        XLll_member * member_now=event_ll->head;
        for(int i=0;i<event_ll->member_num;i++){
            XLevent * event_now=(XLevent*)member_now->data;
            if(event_now->sign.use==ENABLE&&event_now->sign.name!=NULL
                &&strcmp(ins->receiver.name,event_now->sign.name)==0){
                XLmonitor * monitor=monitor_get_by_id(event_now->mon_id);
                if(monitor!=NULL)queue_add_ins(&monitor->queue_head,ins,0);
            }
            member_now=member_now->next;
        }
        return 1;
    }
    //添加指令到各个监视器中
    extern XLll * monitor_ll;
    if(monitor_ll==NULL)return -1;
    XLll_member * member_now=monitor_ll->head;
    for(int i=0;i<monitor_ll->member_num;i++)
    {
        XLmonitor * monitor_now=(XLmonitor*)member_now->data;
        int num=0;
        XLsource *source;
        source=&monitor_now->receiver;

        if(AC&&monitor_now->id==SA_mon_id)num++;   //启动应用的时候
        else if(source->mode==ins->receiver.mode){
            if((source->mode==EVENT_ID||source->mode==ACCESS||source->mode==CORE_MYSELF||source->mode==CORE_OTHER)
                &&source->id==ins->receiver.id)num++;
            if(source->mode==SIGN_NAME&&strcmp(source->name,ins->receiver.name)==0){
                event_id_t event_id=event_get_by_signname(source->name);
                XLevent * event=event_get_by_id(event_id);
                if(event==NULL)return -1;

                num++;
            }
        }

        if(num!=1){
            member_now=member_now->next;
            continue;
        }
        /*XLll *source_ll=&monitor_now->source_ll;
        if(source_ll->head==NULL)queue_add_ins(&monitor_now->queue_head,ins,0);
        XLll_member * member=source_ll->head;
        for(int i=0;i<source_ll->member_num;i++){
            XLsource * source=(XLsource*)member->data;
            if(source_cmp(source,&ins->sender)==1){
        */
        queue_add_ins(&monitor_now->queue_head,ins,0);

        /*        break;
           }
            member=member->next;
        }*/
        member_now=member_now->next;
    }

    return 1;
}
//----------------------------------------------//
//            指令监视器的基本操作                 //
//----------------------------------------------//
mon_id_t monitor_create(XLsource * receiver)
{
    if(receiver==NULL)return -1;

    extern XLll * monitor_ll;
    if(monitor_ll==NULL)monitor_ll=ll_create(sizeof(XLmonitor));
    XLmonitor monitor_new;
    monitor_new.id=1;
    monitor_new.source_ll=*ll_create(sizeof(XLsource));
    monitor_new.queue_head.queue=NULL;
    monitor_new.receiver=*receiver;
    //添加首个成员
    if(monitor_ll->head==NULL){
        ll_add_member_head(monitor_ll,&monitor_new,sizeof(XLmonitor));
        return 1;
    }
    //添加成员
    XLll_member * member_now=monitor_ll->head;
    int mode=0,mem_front_id=-1;//-1代表位于首位
    //获得新的成员的ID和要插入的位置
    for(int i=0;i<monitor_ll->member_num;i++){
        XLmonitor * monitor_now=(XLmonitor*)member_now->data;
        if(mode==0&&monitor_new.id==monitor_now->id){
            monitor_new.id++;
            mem_front_id=i;
        }
        else if(mode==1&&monitor_new.id==monitor_now->id){
            monitor_new.id++;
            mem_front_id=i;
            mode=0;
        }
        else if(mode==0)mode=1;

        member_now=member_now->next;
    }

    if(mem_front_id==-1)ll_insert_member_front(monitor_ll,&monitor_new,sizeof(XLmonitor),0);
    else ll_insert_member_next(monitor_ll,&monitor_new,sizeof(XLmonitor),mem_front_id);
    return monitor_new.id;

}

void monitor_show(void)
{
    printf("monitor show:\n");
    extern XLll * monitor_ll;
    if(monitor_ll==NULL)return;
    XLll_member * member_now=monitor_ll->head;
    if(member_now==NULL)return;
    for(int i=0;i<monitor_ll->member_num;i++)
    {
        XLmonitor * monitor_now=(XLmonitor*)member_now->data;
        printf("%d ",monitor_now->id);
        member_now=member_now->next;
    }
    printf("\n");
}

int monitor_remove(mon_id_t id)
{
    extern XLll * monitor_ll;
    if(monitor_ll==NULL)return -1;
    XLll_member * member_now=monitor_ll->head;
    for(int i=0;i<monitor_ll->member_num;i++){
        XLmonitor * monitor_now=(XLmonitor*)member_now->data;
        if(monitor_now->id==id){
            monitor_remove_all_member(monitor_now->id);
            ll_del_member(monitor_ll,i);
            return 1;
        }
        member_now=member_now->next;
    }
    return -1;
}

XLmonitor * monitor_get_by_id(mon_id_t id){
    extern XLll * monitor_ll;
    if(monitor_ll==NULL)return NULL;
    XLll_member * member_now=monitor_ll->head;
    for(int i=0;i<monitor_ll->member_num;i++){
        XLmonitor * monitor_now=(XLmonitor*)member_now->data;
        if(monitor_now->id==id)return monitor_now;
        member_now=member_now->next;
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
    return monitor->queue_head.queue->in.pak_ins.ins;
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
/*
int monitor_limit_add_source(mon_id_t mon_id,XLsource * source){
    XLmonitor * monitor=monitor_get_by_id(mon_id);
    if(monitor==NULL)return -1;
    return source_ll_add_source(&monitor->source_ll,source);
}

int monitor_limit_remove_source(mon_id_t mon_id,XLsource * source){
    XLmonitor * monitor=monitor_get_by_id(mon_id);
    if(monitor==NULL)return -1;
    return source_ll_remove_source(&monitor->source_ll,source);
}

int monitor_limit_remove_all_source(mon_id_t mon_id){
    XLmonitor * monitor=monitor_get_by_id(mon_id);
    if(monitor==NULL)return -1;
    return source_ll_free_all(&monitor->source_ll);
}
*/
//----------------------------------------------//
//            对指令的参数进行提取                 //
//----------------------------------------------//
int ins_get_par(INS * ins,char *name){
    XLins_decoded * decode=ins_decode(ins);
    for(int i=0;i<decode->argc;i++){
        if(strcmp(name,(char*)decode->argv[i])==0){free(decode);return 1;}
    }
    return 0;
}

char * ins_get_par_str(INS * ins,char *name){
    XLins_decoded * decode=ins_decode(ins);
    for(int i=0;i<decode->argc;i++){
        if(strcmp(name,(char*)decode->argv[i])==0&&i+1<=decode->argc)
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

//192.168.1.16:8081:EID:11
//192.168.1.16:8081:SIGN:test
XLsource * ins_get_par_source(INS * ins,char * name){
    char * par=ins_get_par_str(ins,name);
    if(par==NULL)return NULL;
    char * str[4],*str_p;
    int p=0;
    for(int i=0;i<4;i++)str[i]=(char*)malloc(sizeof(char)*strlen(par));
    for(int i=0;i<4;i++){
        str_p=str[i];
        for(int j=p;j<strlen(par)+1;j++){
            if(j>=strlen(par)+1){
                for(int i=0;i<4;i++)free(str[i]);
                return NULL;
            }
            if(par[j]==':'||par[j]=='\0'){
                str[i][j]='\0';
                p=j+1;
                break;
            }
            *str_p=par[j];
            str_p++;
        }
    }
    //for(int i=0;i<4;i++)printf("%s\n",str[i]);
    XLsource * source=(XLsource*)malloc(sizeof(XLsource));
    source->net.ip=inet_addr(str[0]);
    source->net.port=atoi(str[1]);
    free(str[0]);
    free(str[1]);
    if(strcmp(str[2],"EID")==0){    //EVENT_ID
        source->mode=EVENT_ID;
        source->id=atoi(str[3]);
        free(str[3]);
    }
    else if(strcmp(str[2],"SN")==0){     //SIGN
        source->mode=SIGN_NAME;
        source->name=str[3];
    }
    else if(strcmp(str[2],"CM")==0){     //CORE_MYSELF
        source->mode=CORE_MYSELF;
        source->id=atoi(str[3]);
        free(str[3]);
    }
    else if(strcmp(str[2],"CO")==0){     //CORE_OTHOR
        source->mode=CORE_OTHER;
        source->id=atoi(str[3]);
        free(str[3]);
    }
    else {
        free(source);
        return NULL;
    }
    free(str[2]);
    return source;
}

int source_cmp(XLsource *source1,XLsource *source2){
    int i=0;
    if(source1->mode==source2->mode)i++;
    if((source1->mode==EVENT_ID||source1->mode==ACCESS)&&source1->id==source2->id)i+=1;
    if(source1->mode==SIGN_NAME&&strcmp(source1->name,source2->name)==0)i+=1;
    if(source1->net.ip==source2->net.ip)i+=1;
    if(i==3)return 1;
    return 0;
}

int source_cpy(XLsource * source1,XLsource * source2){
    if(source1==NULL||source2==NULL)return -1;
    source1->id=source2->id;
    source1->mode=source2->mode;
    source1->net.ip=source2->net.ip;
    source1->net.port=source2->net.port;
    if(source2->mode==SIGN_NAME||source2->mode==START_APP){
        if(source2->name==NULL)return -1;
        source1->name=(char*)malloc(strlen(source2->name)+1);
        strcpy(source1->name,source2->name);
    }
    return 1;
}

int source_ll_add_source(XLll * source_ll,XLsource * source){
    if(source==NULL||source_ll==NULL)return -1;
    //if(source_ll->head==NULL)return -1;
    if(source_ll_find_source(source_ll,source)>0)return -2;
    ll_add_member_tail(source_ll,source,sizeof(XLsource));
    return 1;

}

int source_ll_remove_source(XLll * source_ll,XLsource * source){
    if(source==NULL)return -1;
    if(source_ll->head==NULL)return 2;

    XLll_member * member_now=source_ll->head;
    for(int i=0;i<source_ll->member_num;i++){
        XLsource *source_now=(XLsource*)member_now->data;
        if(source_cmp(source_now,source)==1){
            member_now=member_now->next;
            ll_del_member(source_ll,i);
        }
        else member_now=member_now->next;
    }
    return 1;
}

int source_ll_free_all(XLll * source_ll){
    if(source_ll==NULL||source_ll->head==NULL)return 1;

    ll_del_member_all(source_ll);
    return 1;    
}

int source_ll_find_source(XLll * source_ll,XLsource * source){
    if(source==NULL)return -1;
    if(source_ll->head==NULL)return -1;

    XLll_member * member_now=source_ll->head;
    for(int i=0;i<source_ll->member_num;i++){
        XLsource *source_now=(XLsource*)member_now->data;
        if(source_cmp(source_now,source)==1){
            return 1;
        }
        else member_now=member_now->next;
    }
    return -1;
}
