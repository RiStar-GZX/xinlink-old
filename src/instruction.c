#include<instruction.h>

INS * ins_encode(XLins_decoded * decoded_ins)
{
    //Error
    if(decoded_ins==NULL)return 0;
    //if(strcmp((str*)uncode_ins->event_name," ")==0)return 0;

    //Measurement data size
    int ins_size=1;
    ins_size+=strlen((str*)decoded_ins->event_name);
    XLins_decoded_data *data_now=decoded_ins->data_list;
    while (data_now!=NULL) {
        //if(strcmp((str*)data_now->data_name," ")==0)return 0;

        ins_size+=strlen((str*)data_now->data_name)+1;
        if(data_now->datasize>0)ins_size+=data_now->datasize+1;

        if(data_now->next==NULL)break;
        data_now=data_now->next;
    }
    printf("size:%d\n",ins_size);

    //Encode uncoded data
    INS * ins_out=malloc(sizeof (INS)*ins_size);
    strcat(ins_out,(str*)&decoded_ins->event_name);
    data_now=decoded_ins->data_list;
    while (data_now!=NULL) {
        strcat(ins_out," ");
        strcat(ins_out,(str*)&data_now->data_name);

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

str * get_str(str * s,int * p,int maxsize)
{
    if(*p>=maxsize)return 0;
    str * s_p=s,*new=malloc(sizeof(str)*(maxsize-*p)),*new_p=new;
    int mode=0,i=0;
    s_p+=*p;
    while (1) {
        if(*s_p==' '&&mode==0){
            s_p++;
            i++;
        }
        else if((*s_p==' '||*s_p=='\0')&&mode==1)
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
    new_p++;
    *new_p='\0';
    return new;
}

XLins_decoded * ins_decode(INS * ins)
{
    if(ins==NULL)return 0;
    XLins_decoded * decode=malloc(sizeof (XLins_decoded));
    int p=0;
    strcpy((str*)decode->event_name,get_str(ins,&p,strlen(ins)));
    if(p>=strlen(ins)){
        decode->data_list=NULL;
        return decode;
    }
    decode->data_list=malloc(sizeof (XLins_decoded_data));
    XLins_decoded_data * data_now=decode->data_list;
    while (1) {
        if(p>=strlen(ins))break;
        strcpy((str*)data_now->data_name,get_str(ins,&p,strlen(ins)));

    }
    return NULL;
}

