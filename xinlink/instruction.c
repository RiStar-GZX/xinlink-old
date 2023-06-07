#include<instruction.h>
#include <network.h>
#include <event.h>

INS * ins_encode(XLins_decoded * decoded_ins)
{
    //Error
    if(decoded_ins==NULL)return 0;
    //if(strcmp((str*)uncode_ins->event_name," ")==0)return 0;

    //Measurement data size
    int ins_size=1;
    ins_size+=strlen((str*)decoded_ins->app_name);
    XLins_decoded_data *data_now=decoded_ins->data_list;
    while (data_now!=NULL) {
        //if(strcmp((str*)data_now->data_name," ")==0)return 0;
        //ins_size+=strlen((str*)data_now->data_name)+1;
        if(data_now->datasize>0)ins_size+=data_now->datasize+1;

        if(data_now->next==NULL)break;
        data_now=data_now->next;
    }
    printf("size:%d\n",ins_size);

    //Encode uncoded data
    INS * ins_out=malloc(sizeof (INS)*ins_size);
    strcat(ins_out,(str*)&decoded_ins->app_name);
    data_now=decoded_ins->data_list;
    while (data_now!=NULL) {
        //strcat(ins_out," ");
        //strcat(ins_out,(str*)&data_now->data_name);

        if(data_now->datasize>0)
        {
            strcat(ins_out," ");
            strncat(ins_out,(str*)data_now->data,data_now->datasize);
        }

        if(data_now->next==NULL)break;
        data_now=data_now->next;
    }
    return ins_out;
}

str * get_ins(str * s,int * p,int maxsize,int * size)
{
    if(*p>=maxsize)return 0;
    str * s_p=s,*new=malloc(sizeof(str)*(maxsize-*p)),*new_p=new;
    int mode=0,i=0,datasize=0;
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
            datasize++;
            mode=1;
        }
        if(*p>maxsize)return 0;
    }
    //write endl
    new_p++;
    *new_p='\0';
    if(size!=NULL)*size=datasize;
    return new;
}

XLins_decoded * ins_decode(INS * ins)
{
    //ERROR
    if(ins==NULL)return 0;

    int p=0;
    XLins_decoded * decode=malloc(sizeof (XLins_decoded));

    //Copy event_name
    strcpy((str*)decode->app_name,get_ins(ins,&p,strlen(ins),NULL));

    //No data
    if(p>=(int)strlen(ins)){
        decode->data_list=NULL;
        return decode;
    }

    //Have data
    decode->data_list=malloc(sizeof (XLins_decoded_data));
    XLins_decoded_data * data_now=decode->data_list;
    while (1) {

        if(p>=(int)strlen(ins))break;
        data_now->data=(uint8_t *)get_ins(ins,&p,(int)strlen(ins),&data_now->datasize);
        if(p>=(int)strlen(ins))break;

        //New data
        data_now->next=malloc(sizeof (XLins_decoded_data));
        data_now=data_now->next;
        data_now->next=NULL;
    }
    return decode;
}

//send ins to (start) event
int ins_send_to_event(XLins * ins){
    int sender_mode,receiver_mode;
    sender_mode=ins->mode&SEE_SENDER_ONLY;
    receiver_mode=ins->mode&SEE_RECEIVER_ONLY;
    printf("1\n");
    if(receiver_mode==NETWORK_MODE_RECEIVER_START_EVENT)
    {

        XLapp * app=app_get_by_name(ins->receiver.name);
        if(app==NULL)return -1;


        event_id_t event_id=event_create(app->event);
        if(event_id<=0)return -1;
        XLevent_list *event_list=event_get_by_id(event_id);

        if(event_list==NULL)return -1;
        printf("2\n");
        queue_show(&event_list->head);
        //queue_add(&event_list->head,ins,0);

        printf("2\n");
        event_run(event_id);
    }
    if(receiver_mode==NETWORK_MODE_RECEIVER_EVENT_ID)
    {
        EVENT event;
        event=event_get_by_id(ins->receiver.id);
    }
    return 1;
}

int ins_sender_set_id(XLins * ins,int mode,int id){
    if(ins==NULL)return -1;
    if(mode!=MODE_EVENT_ID&&mode!=MODE_DEV_ID&&mode!=MODE_ACCESS_ID)return -1;
    ins->sender.mode=mode;
    ins->sender.id=id;
    return id;
}

int ins_recevier_set_id(XLins * ins,int mode,int id){
    if(ins==NULL)return -1;
    if(mode!=MODE_EVENT_ID&&mode!=MODE_DEV_ID&&mode!=MODE_ACCESS_ID)return -1;
    ins->receiver.mode=mode;
    ins->receiver.id=id;
    return id;
}

int ins_recevier_set_appname(XLins * ins,str *name){
    if(ins==NULL)return -1;
    ins->receiver.mode=MODE_APP_NAME;
    strcpy(ins->receiver.name,name);
    return 1;
}

XLins_queue_head * monitor(XLsource * sender,XLsource *recevier)
{
    extern XLins_queue * total_queue_head;
    XLins_queue * queue_now=total_queue_head;
    if(queue_now==NULL)return NULL;
    XLins_queue_head * queue_head=malloc(sizeof (XLins_queue_head));
    XLsource * sender_now;
    XLsource * recevier_now;
    while (1) {
        int a=0;
        sender_now=&queue_now->ins->sender;
        recevier_now=&queue_now->ins->receiver;
        if(sender==NULL)a++;
        else if(sender_now->mode==MODE_APP_NAME&&strcmp(sender_now->name,sender->name)==0)a++;
        else if(sender_now->mode==sender->mode&&sender_now->id==sender->id)a++;
        else a=0;

        if(recevier==NULL)a++;
        else if(recevier_now->mode==MODE_APP_NAME&&strcmp(recevier_now->name,recevier->name)==0)a++;
        else if(recevier_now->mode==recevier->mode&&recevier_now->id==recevier->id)a++;
        else a=0;

        if(a==2)queue_add(queue_head,queue_now->ins,0);
        if(queue_now->next==total_queue_head)break;
        queue_now=queue_now->next;
    }
    return queue_head;
}

