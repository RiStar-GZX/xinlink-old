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

    if(receiver_mode==NETWORK_MODE_RECEIVER_START_EVENT)
    {
        XLapp * app=app_get_by_id(ins->core_id);
        if(app==NULL)return -1;
        //app->event(ins);
        printf("here!\n");
        event_id_t event_id=event_create(app->event);

        //if(event_id<=0)return -1;
        //event_run(event_id,ins);
    }
    if(receiver_mode==NETWORK_MODE_RECEIVER_DEV|receiver_mode==NETWORK_MODE_RECEIVER_DEV_EVENT)
    {

    }
    if(receiver_mode==NETWORK_MODE_RECEIVER_EXIST_EVENT)
    {
    }
}
