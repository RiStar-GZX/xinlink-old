/**
 * Copyright (C), 2022-2023, github.com/Ristar-GZX.
 * File name: device.c
 * Author:Ristar-GZX  Version:V0.0.1    Date:2023.1.5
 * Description:
 * Others:
 * Function List:
    1. main
 * History:
    1. Date:
       Author:
       Modification:
*/
#include<device.h>

XLdevice * device_head=NULL;//&root.device_head;
XLnetdev * netdev_head=NULL;//&root.netdev_head;
XLrelate * relate_head=NULL;


/*device*/
XLdevice * device_get(dev_op_t dev_op) {
        extern XLdevice * device_head;
        XLdevice * device_now=device_head;
        for(;;) {
                if(device_now->op==dev_op)
                        return device_now;

                else if(device_now->next_op!=NULL)
                        device_now=device_now->next_op;
                else return NULL;
        }
}

dev_op_t device_create(void) {
        XLdevice * device_new=malloc(sizeof(XLdevice));
        extern XLdevice * device_head;

        if(device_head==NULL) {
                device_head=device_new;
                device_new->op=1;
                device_new->next_op=NULL;
                return device_new->op;
        } else if(device_head->op>1) {
                device_new->op=1;
                XLdevice * temp;
                temp=device_head;
                device_head=device_new;
                device_new->next_op=temp;
                return device_new->op;
        } else {
                XLdevice * device_now;

                device_now=device_head;

                for(;;) {
                        if(device_now->next_op==NULL)  {

                                device_now->next_op=device_new;
                                device_new->op=device_now->op+1;
                                device_new->next_op=NULL;

                                return device_new->op;
                        } else if(device_now->op+1<device_now->next_op->op) { //如果下一个设备的操作符与当前设备操作符有空位

                                device_new->op=device_now->op+1;            //新设备占据这个操作符

                                device_new->next_op=device_now->next_op;       //设置新设备的下一个设备

                                device_now->next_op=device_new;             //设置现在设备的下一个设备为新设备

                                return device_new->op;
                        } else  device_now=device_now->next_op;  //下一个

                }

        }

}

dev_op_t device_del(dev_op_t dev_op) {
        extern XLdevice * device_head;
        XLdevice * device_now=device_head;
        XLdevice * device_nowfront=NULL;
        for(;;) {
                if(device_now->op==dev_op) {            //如果有这个设备
                        if(device_now==device_head) {           //如果这个设备是设备头
                                if(device_head->next_op==NULL) {        //如果就剩这一个设备
                                        //free(device_head);                    //释放内存
                                        device_head=NULL;                       //释放指针
                                        return 1;                                 //成功返回
                                } else {                                //如果还有其他设备
                                        XLdevice * temp;
                                        temp=device_head->next_op;              //将设备头赋予下一个设备
                                        //free(device_head);                    //释放内存
                                        device_head=temp;
                                        return 1;                                 //成功返回
                                }
                        } else {                                //如果这个设备不是头
                                if(device_now->next_op==NULL) {         //如果这个设备后面没有设备了
                                        free(device_now);                       //释放内存
                                        device_nowfront->next_op=NULL;          //删除前面一个设备的后端指针
                                        return 1;                                 //成功返回
                                } else {                                //如果没有设备了
                                        XLdevice * temp;
                                        temp=device_now->next_op;
                                        device_nowfront->next_op=temp;          //前面一个设备的后端指针指向后一个设备
                                        free(device_now);                       //释放内存
                                        return 1;                                 //成功返回
                                }
                        }
                } else if(device_now->next_op!=NULL) {          //如果不是要找的设备但后面还有设备
                        device_nowfront=device_now;                     //记录下当前操作设备的前一个设备
                        device_now=device_now->next_op;                 //当前操作设备向后移动一个
                } else return -1;                               //如果既不是要找的设备后面也没有设备了，报错退出
        }
}

int8_t device_init(dev_op_t dev_op,str * name,init_type_t type) {
        if(type==DEVICE_TYPE_SN) {
                sig_op_t signal_base;
                signal_base=signal_create();

                if(device_set_name(dev_op,name)<0) {		//设置信号归属设备
                        printf("set_dev_name error!\n");
                        return -1;
                }
                printf("set_dev_name success!\n");



                if(signal_set_name(signal_base,"snbase")<0) {	//基础信号设置
                        printf("set_name error!\n");
                        return -1;
                }
                printf("set_name success!\n");

                if(signal_add_par(signal_base,"mode")<0) {		//添加信号参数
                        printf("set_par_name error!\n");
                        return -1;
                }
                printf("set_par_name success!\n");

                /*if(signal_del_par(signal_base,"mode")<0) {		//添加信号参数
                        printf("del_par_name error!\n");
                        return -1;
                }
                printf("del_par_name success!\n");*/



                if(signal_set_dev(signal_base,dev_op)<0) {		//设置信号归属设备
                        printf("set_dev error!\n");
                        return -1;
                }
                return 0;
        }else return -1;
}

void device_list(void) {
        extern XLdevice * device_head;
        XLdevice * device_now=device_head;
        while(device_now!=NULL) {
                printf("%d ",device_now->op);
                device_now=device_now->next_op;
        }
}

sig_op_t device_get_signal(dev_op_t dev_op,str *name)
{
    XLdevice * device=device_get(dev_op);
    if(device->signal_h!=NULL)
    {
        XLdevice_sig_list * signal_now=device->signal_h;
        for(;;){
            if(strcmp(name,signal_now->signal->name)==0)return signal_now->signal->op;
            else if(signal_now->next!=NULL)signal_now=signal_now->next;
            else return 0;
        }
    }else
    {
        return 0;
    }
}

dev_op_t device_get_byname(str * name)
{
    extern XLdevice * device_head;
    if(device_head!=NULL)
    {
        XLdevice * device_now=device_head;
        for(;;)
        {
            if(strcmp(name,device_now->name)==0)return device_now->op;
            else if(device_now->next_op!=NULL) device_now=device_now->next_op;
            else return -1;
        }
    }else{
        return -1;
    }
}

int8_t device_set_name(dev_op_t dev_op,str * name)
{
    XLdevice * device=device_get(dev_op);
    if(device_get_byname(name)<=0)
    {
        return -1;
    }else{
        strcpy(device->name,name);
        return 1;
    }
}


/*netdevice */
XLnetdev * netdev_get(netdev_op_t netdev_op) {
        extern XLnetdev * netdev_head;
        XLnetdev * netdev_now=netdev_head;
        for(;;) {
                if(netdev_now->op==netdev_op)
                        return netdev_now;

                else if(netdev_now->next_op!=NULL)
                        netdev_now=netdev_now->next_op;
                else return NULL;
        }
}

netdev_op_t netdev_create(void) {
        XLnetdev * netdev_new=malloc(sizeof(XLnetdev));
        extern XLnetdev * netdev_head;

        if(netdev_head==NULL) {
                netdev_head=netdev_new;
                netdev_new->op=1;
                netdev_new->next_op=NULL;
                return netdev_new->op;
        } else if(netdev_head->op>1) {
                netdev_new->op=1;
                XLnetdev * temp;
                temp=netdev_head;
                netdev_head=netdev_new;
                netdev_new->next_op=temp;
                return netdev_new->op;
        } else {
                XLnetdev * netdev_now;

                netdev_now=netdev_head;

                for(;;) {
                        if(netdev_now->next_op==NULL)  {

                                netdev_now->next_op=netdev_new;
                                netdev_new->op=netdev_now->op+1;
                                netdev_new->next_op=NULL;

                                return netdev_new->op;
                        } else if(netdev_now->op+1<netdev_now->next_op->op) { //如果下一个设备的操作符与当前设备操作符有空位

                                netdev_new->op=netdev_now->op+1;            //新设备占据这个操作符

                                netdev_new->next_op=netdev_now->next_op;       //设置新设备的下一个设备

                                netdev_now->next_op=netdev_new;             //设置现在设备的下一个设备为新设备

                                return netdev_new->op;
                        } else  netdev_now=netdev_now->next_op;  //下一个

                }

        }

}

netdev_op_t netdev_del(netdev_op_t dev_op) {
        extern XLnetdev * netdev_head;
        XLnetdev * netdev_now=netdev_head;
        XLnetdev * netdev_nowfront=NULL;
        for(;;) {
                if(netdev_now->op==dev_op) {            //如果有这个设备
                        if(netdev_now==netdev_head) {           //如果这个设备是设备头
                                if(netdev_head->next_op==NULL) {        //如果就剩这一个设备
                                        //free(netdev_head);                    //释放内存
                                        netdev_head=NULL;                       //释放指针
                                        return 1;                                 //成功返回
                                } else {                                //如果还有其他设备
                                        XLnetdev * temp;
                                        temp=netdev_head->next_op;              //将设备头赋予下一个设备
                                        //free(netdev_head);                    //释放内存
                                        netdev_head=temp;
                                        return 1;                                 //成功返回
                                }
                        } else {                                //如果这个设备不是头
                                if(netdev_now->next_op==NULL) {         //如果这个设备后面没有设备了
                                        free(netdev_now);                       //释放内存
                                        netdev_nowfront->next_op=NULL;          //删除前面一个设备的后端指针
                                        return 1;                                 //成功返回
                                } else {                                //如果没有设备了
                                        XLnetdev * temp;
                                        temp=netdev_now->next_op;
                                        netdev_nowfront->next_op=temp;          //前面一个设备的后端指针指向后一个设备
                                        free(netdev_now);                       //释放内存
                                        return 1;                                 //成功返回
                                }
                        }
                } else if(netdev_now->next_op!=NULL) {          //如果不是要找的设备但后面还有设备
                        netdev_nowfront=netdev_now;                     //记录下当前操作设备的前一个设备
                        netdev_now=netdev_now->next_op;                 //当前操作设备向后移动一个
                } else return -1;                               //如果既不是要找的设备后面也没有设备了，报错退出
        }
}

void netdev_list(void) {
        extern XLnetdev * netdev_head;
        XLnetdev * netdev_now=netdev_head;
        while(netdev_now!=NULL) {
                printf("%d ",netdev_now->op);
                netdev_now=netdev_now->next_op;
        }
}

netdev_op_t netdev_get_byname(str * name)
{
    extern XLnetdev * netdev_head;
    if(netdev_head!=NULL)
    {
        XLnetdev * netdev_now=netdev_head;
        for(;;)
        {
            if(strcmp(name,netdev_now->name)==0)return netdev_now->op;
            else if(netdev_now->next_op!=NULL) netdev_now=netdev_now->next_op;
            else return -1;
        }
    }else{
        return -1;
    }
}

int8_t netdev_set_name(netdev_op_t netdev_op,str * name)
{
    XLnetdev * netdev=netdev_get(netdev_op);
    if(netdev_get_byname(name)<=0)
    {
        return -1;
    }else{
        strcpy(netdev->name,name);
        return 1;
    }
}


int8_t netdev_set_net(netdev_op_t op,ip ip,uint16_t port,uint8_t family)
{
    XLnetdev * netdev=netdev_get(op);
    if(netdev==NULL)return -1;
    netdev->net.ip=ip;
    netdev->net.family=family;
    netdev->net.port=port;
    return 1;
}
/*relate*/
int8_t relate_set(op_t dev1,dev_type_t type1,op_t dev2,dev_type_t type2,mod_t hard,mod_t soft)
{
    if(type1==DEV_TYPE_LO){if(device_get(dev1)==NULL)return -1;}
    else if(type1==DEV_TYPE_NET){if(netdev_get(dev1)==NULL)return -1;}
    else return -1;
    if(type2==DEV_TYPE_LO){if(device_get(dev2)==NULL)return -1;}
    else if(type2==DEV_TYPE_NET){if(netdev_get(dev2)==NULL)return -1;}
    else return -1;

    if(type1==type2)
    {
        if(type1==DEV_TYPE_LO)if(device_get(dev1)==device_get(dev2))return -1;
        if(type1==DEV_TYPE_NET)if(netdev_get(dev1)==netdev_get(dev2))return -1;
    }
    if(hard>2||hard<0)return -1;
    if(soft>2||soft<0)return -1;

    extern XLrelate * relate_head;
    XLrelate *relate_now=relate_head,*relate_new=NULL;
    if(relate_head==NULL)
    {
        relate_new=malloc(sizeof(struct XLrelate));
        relate_head=relate_new;
    }
    else{
        int a=1;
        while(a!=0){
            if(relate_now->op1==dev1&&relate_now->dev1_type==type1&&relate_now->op2==dev2&&relate_now->dev2_type==type2){
                relate_new=relate_now;
                a=0;
            }
            else if(relate_now->next==NULL)
            {
                relate_new=malloc(sizeof(struct XLrelate));
                relate_now->next=relate_new;
                a=0;
            }
            relate_now=relate_now->next;
        }
        //if(a==0)relate_new=relate_now;
        /*else if(a==2) {relate_new=malloc(sizeof(struct XLrelate));
        relate_now->next=relate_new;}*/
    }
    relate_new->op1=dev1;
    relate_new->op2=dev2;
    relate_new->dev1_type=type1;
    relate_new->dev2_type=type2;
    relate_new->hard=hard;
    relate_new->soft=soft;
    return 1;
}
int8_t relate_del(op_t dev1,dev_type_t type1,op_t dev2,dev_type_t type2)
{
    extern XLrelate * relate_head;
    XLrelate * relate_front=NULL,*relate_now=relate_head;
    while(relate_now!=NULL)
    {
        if(relate_now->op1==dev1&&relate_now->dev1_type==type1&&relate_now->op2==dev2&&relate_now->dev2_type==type2)
        {
            if(relate_front==NULL){
                if(relate_now->next==NULL)relate_head=NULL;
                if(relate_now->next!=NULL)relate_head=relate_now->next;
            }
            else if(relate_front!=NULL){
                if(relate_now->next==NULL)relate_front->next=NULL;
                if(relate_now->next!=NULL)relate_front->next=relate_now->next;
            }
            free(relate_now);
        }
        relate_front=relate_now;
        relate_now=relate_now->next;
    }
    return -1;
}
void relate_list(void)
{
    //printf("\f");
    extern XLrelate * relate_head;
    XLrelate * relate_now=relate_head;
    if(relate_head==NULL)printf("no relate\n");
    while(relate_now!=NULL)
    {
        if(relate_now->dev1_type==DEV_TYPE_LO)printf("LO:");
        if(relate_now->dev1_type==DEV_TYPE_NET)printf("NET:");
        printf("%d ",relate_now->op1);
        printf("(");
        if(relate_now->hard==RELATE_HARD_1p)printf("H");
        if(relate_now->soft==RELATE_SOFT_1p)printf("S");
        printf(")");
        if(relate_now->dev2_type==DEV_TYPE_LO)printf("LO:");
        if(relate_now->dev2_type==DEV_TYPE_NET)printf("NET:");
        printf("%d ",relate_now->op2);
        printf("(");
        if(relate_now->hard==RELATE_HARD_2p)printf("H");
        if(relate_now->soft==RELATE_SOFT_2p)printf("S");
        printf(")");
        printf("\n");
        relate_now=relate_now->next;
    }
}
