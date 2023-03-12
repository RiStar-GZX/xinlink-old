/**
 * Cidyright (C), 2022-2023, github.com/Ristar-GZX.
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

XLdev * dev_head=NULL;//&root.dev_head;
XLnetdev * netdev_head=NULL;//&root.netdev_head;
XLrelate * relate_head=NULL;


/*name*/

int8_t name_legality(str * name)
{
    //if(strcpy(name,"")==0){printf("legality");return 0;}
    return 1;
}

/*dev*/
XLdev * dev_get(dev_id_t dev_id) {
        extern XLdev * dev_head;
        XLdev * dev_now=dev_head;
        for(;;) {
                if(dev_now->id==dev_id)
                        return dev_now;

                else if(dev_now->next!=NULL)
                        dev_now=dev_now->next;
                else return NULL;
        }
}

dev_id_t dev_create(str * name) {
        if(name_legality(name)<=0)return 0; //名字合法性判断
        if(dev_get_byname(name)!=0)         //判断是否有重名设备
        {
            printf("same name!\n");
            return 0;
        }
        XLdev * dev_new=malloc(sizeof(XLdev));
        memset(dev_new,0,sizeof(XLdev));
        extern XLdev * dev_head;
        /****************/
        if(dev_head==NULL) {
                dev_head=dev_new;
                dev_new->id=1;
                dev_new->next=NULL;
                strcpy(dev_new->name,name);
                return dev_new->id;
        } else if(dev_head->id>1) {
                dev_new->id=1;
                XLdev * temp;
                temp=dev_head;
                dev_head=dev_new;
                dev_new->next=temp;
                strcpy(dev_new->name,name);
                return dev_new->id;
        } else {
                XLdev * dev_now;

                dev_now=dev_head;

                for(;;) {
                        if(dev_now->next==NULL)  {

                                dev_now->next=dev_new;
                                dev_new->id=dev_now->id+1;
                                dev_new->next=NULL;
                                strcpy(dev_new->name,name);
                                return dev_new->id;
                        } else if(dev_now->id+1<dev_now->next->id) { //如果下一个设备的操作符与当前设备操作符有空位

                                dev_new->id=dev_now->id+1;            //新设备占据这个操作符

                                dev_new->next=dev_now->next;       //设置新设备的下一个设备

                                dev_now->next=dev_new;             //设置现在设备的下一个设备为新设备
                                strcpy(dev_new->name,name);
                                return dev_new->id;
                        } else  dev_now=dev_now->next;  //下一个

                }

        }

}

dev_id_t dev_del(dev_id_t dev_id) {
        extern XLdev * dev_head;
        XLdev * dev_now=dev_head;
        XLdev * dev_nowfront=NULL;
        for(;;) {
                if(dev_now->id==dev_id) {            //如果有这个设备
                        if(dev_now==dev_head) {           //如果这个设备是设备头
                                if(dev_head->next==NULL) {        //如果就剩这一个设备
                                        free(dev_head);                    //释放内存
                                        dev_head=NULL;                       //释放指针
                                        return 1;                                 //成功返回
                                } else {                                //如果还有其他设备
                                        XLdev * temp;
                                        temp=dev_head->next;              //将设备头赋予下一个设备
                                        free(dev_head);                    //释放内存
                                        dev_head=temp;
                                        return 1;                                 //成功返回
                                }
                        } else {                                //如果这个设备不是头
                                if(dev_now->next==NULL) {         //如果这个设备后面没有设备了
                                        free(dev_now);                       //释放内存
                                        dev_nowfront->next=NULL;          //删除前面一个设备的后端指针
                                        return 1;                                 //成功返回
                                } else {                                //如果没有设备了
                                        XLdev * temp;
                                        temp=dev_now->next;
                                        dev_nowfront->next=temp;          //前面一个设备的后端指针指向后一个设备
                                        free(dev_now);                       //释放内存
                                        return 1;                                 //成功返回
                                }
                        }
                } else if(dev_now->next!=NULL) {          //如果不是要找的设备但后面还有设备
                        dev_nowfront=dev_now;                     //记录下当前操作设备的前一个设备
                        dev_now=dev_now->next;                 //当前操作设备向后移动一个
                } else return 0;                               //如果既不是要找的设备后面也没有设备了，报错退出
        }
}

/*int8_t dev_init(dev_id_t dev_id,str * name,init_type_t type) {
        if(type==DEVICE_TYPE_SN) {
                sig_id_t sig_base;
                sig_base=sig_create();

                if(dev_set_name(dev_id,name)<0) {		//设置信号归属设备
                        printf("set_dev_name error!\n");
                        return 0;
                }
                printf("set_dev_name success!\n");



                if(sig_set_name(sig_base,"snbase")<0) {	//基础信号设置
                        printf("set_name error!\n");
                        return 0;
                }
                printf("set_name success!\n");

                if(sig_add_par(sig_base,"mode")<0) {		//添加信号参数
                        printf("set_par_name error!\n");
                        return 0;
                }
                printf("set_par_name success!\n");

                /*if(sig_del_par(sig_base,"mode")<0) {		//添加信号参数
                        printf("del_par_name error!\n");
                        return 0;
                }
                printf("del_par_name success!\n");


                if(dev_set_sig(sig_base,dev_id)<0) {		//设置信号归属设备
                        printf("set_dev error!\n");
                        return 0;
                }
                return 0;
        }else return 0;
}*/

void dev_list(void) {
        extern XLdev * dev_head;
        XLdev * dev_now=dev_head;
        while(dev_now!=NULL) {
                printf("%d ",dev_now->id);
                dev_now=dev_now->next;
        }
}

sig_id_t dev_get_sig(dev_id_t dev_id,str *name)
{
    if(name_legality(name)<=0)return 0;
    XLdev * dev=dev_get(dev_id);
    if(dev->sig_h!=NULL)
    {
        XLdev_sig_list * sig_now=dev->sig_h;
        for(;;){
            if(strcmp(name,sig_now->sig->name)==0)return sig_now->sig->id;
            else if(sig_now->next!=NULL)sig_now=sig_now->next;
            else return 0;
        }
    }else
    {
        return 0;
    }
}

dev_id_t dev_get_byname(str * name)
{
    if(name_legality(name)<=0)return 0;
    extern XLdev * dev_head;
    if(dev_head!=NULL)
    {
        XLdev * dev_now=dev_head;
        for(;;)
        {
            if(strcmp(name,dev_now->name)==0)return dev_now->id;
            else if(dev_now->next!=NULL) dev_now=dev_now->next;
            else return 0;
        }
    }else{
        return 0;
    }
}

int8_t dev_set_name(dev_id_t dev_id,str * name)
{
    if(name_legality(name)<=0)return 0;
    XLdev * dev=dev_get(dev_id);
    if(dev_get_byname(name)<=0)
    {
        return 0;
    }else{
        strcpy(dev->name,name);
        return 1;
    }
}

int8_t dev_set_sig(dev_id_t dev_id,sig_id_t sig_id) {    //设置信号归属设备
        XLdev * dev=dev_get(dev_id);                   //获得设备结构体
        XLsig * sig=sig_get(sig_id);                   //获得信号结构体
        if(sig!=NULL&&dev!=NULL&&strcmp(sig->name,"")!=0) //判断设备、信号、信号名三者非空
        {
            XLdev_sig_list *dev_sig_now=dev->sig_h;        //设备信号表的操作指针
            if(dev_sig_now!=NULL)                                   //当前设备有信号
            {
                for(;;)
                {                                               //循环找出信号
                    if(strcmp(dev_sig_now->sig->name,sig->name)==0)return 0;
                    else if(dev_sig_now->next!=NULL)dev_sig_now=dev_sig_now->next; //移位下一个
                    else
                    {                                                          //读完了
                        sig->dev=dev;                                      //接到设备的信号表尾
                        dev_sig_now->next=malloc(sizeof(XLdev_sig_list));
                        memset(dev_sig_now,0,sizeof(XLdev_sig_list));
                        dev_sig_now->next->sig=sig;
                        dev_sig_now->next->sig->dev=dev;
                        dev_sig_now->next->next=NULL;
                        return 1;
                    }
                }
            }
            else
            {
                dev->sig_h=malloc(sizeof(XLdev_sig_list));
                memset(dev->sig_h,0,sizeof(XLdev_sig_list));//
                dev->sig_h->next=NULL;
                dev->sig_h->sig=sig;
                sig->dev=dev;
                return 1;
            }
        } else return 0;
}

/*netdev */
XLnetdev * netdev_get(netdev_id_t netdev_id) {
        extern XLnetdev * netdev_head;
        XLnetdev * netdev_now=netdev_head;
        for(;;) {
                if(netdev_now->id==netdev_id)
                        return netdev_now;

                else if(netdev_now->next!=NULL)
                        netdev_now=netdev_now->next;
                else return NULL;
        }
}

netdev_id_t netdev_create(str *name) {
        if(name_legality(name)<=0)return 0; //名字合法性判断
        if(netdev_get_byname(name)!=0)         //判断是否有重名设备
        {
            printf("same name!\n");
            return 0;
        }
        XLnetdev * netdev_new=malloc(sizeof(XLnetdev));
        memset(netdev_new,0,sizeof(XLnetdev));//
        extern XLnetdev * netdev_head;

        if(netdev_head==NULL) {
                netdev_head=netdev_new;
                netdev_new->id=1;
                netdev_new->next=NULL;
                strcpy(netdev_new->name,name);
                return netdev_new->id;
        } else if(netdev_head->id>1) {
                netdev_new->id=1;
                XLnetdev * temp;
                temp=netdev_head;
                netdev_head=netdev_new;
                netdev_new->next=temp;
                strcpy(netdev_new->name,name);
                return netdev_new->id;
        } else {
                XLnetdev * netdev_now;

                netdev_now=netdev_head;

                for(;;) {
                        if(netdev_now->next==NULL)  {

                                netdev_now->next=netdev_new;
                                netdev_new->id=netdev_now->id+1;
                                netdev_new->next=NULL;
                                strcpy(netdev_new->name,name);
                                return netdev_new->id;
                        } else if(netdev_now->id+1<netdev_now->next->id) { //如果下一个设备的操作符与当前设备操作符有空位

                                netdev_new->id=netdev_now->id+1;            //新设备占据这个操作符

                                netdev_new->next=netdev_now->next;       //设置新设备的下一个设备

                                netdev_now->next=netdev_new;             //设置现在设备的下一个设备为新设备
                                strcpy(netdev_new->name,name);
                                return netdev_new->id;
                        } else  netdev_now=netdev_now->next;  //下一个

                }

        }

}

netdev_id_t netdev_del(netdev_id_t dev_id) {
        extern XLnetdev * netdev_head;
        XLnetdev * netdev_now=netdev_head;
        XLnetdev * netdev_nowfront=NULL;
        for(;;) {
                if(netdev_now->id==dev_id) {            //如果有这个设备
                        if(netdev_now==netdev_head) {           //如果这个设备是设备头
                                if(netdev_head->next==NULL) {        //如果就剩这一个设备
                                        free(netdev_head);                    //释放内存
                                        netdev_head=NULL;                       //释放指针
                                        return 1;                                 //成功返回
                                } else {                                //如果还有其他设备
                                        XLnetdev * temp;
                                        temp=netdev_head->next;              //将设备头赋予下一个设备
                                        free(netdev_head);                    //释放内存
                                        netdev_head=temp;
                                        return 1;                                 //成功返回
                                }
                        } else {                                //如果这个设备不是头
                                if(netdev_now->next==NULL) {         //如果这个设备后面没有设备了
                                        free(netdev_now);                       //释放内存
                                        netdev_nowfront->next=NULL;          //删除前面一个设备的后端指针
                                        return 1;                                 //成功返回
                                } else {                                //如果没有设备了
                                        XLnetdev * temp;
                                        temp=netdev_now->next;
                                        netdev_nowfront->next=temp;          //前面一个设备的后端指针指向后一个设备
                                        free(netdev_now);                       //释放内存
                                        return 1;                                 //成功返回
                                }
                        }
                } else if(netdev_now->next!=NULL) {          //如果不是要找的设备但后面还有设备
                        netdev_nowfront=netdev_now;                     //记录下当前操作设备的前一个设备
                        netdev_now=netdev_now->next;                 //当前操作设备向后移动一个
                } else return 0;                               //如果既不是要找的设备后面也没有设备了，报错退出
        }
}

void netdev_list(void) {
        extern XLnetdev * netdev_head;
        XLnetdev * netdev_now=netdev_head;
        while(netdev_now!=NULL) {
                printf("%d ",netdev_now->id);
                netdev_now=netdev_now->next;
        }
}

netdev_id_t netdev_get_byname(str * name)
{
    if(name_legality(name)<=0)return 0;
    extern XLnetdev * netdev_head;
    if(netdev_head!=NULL)
    {
        XLnetdev * netdev_now=netdev_head;
        for(;;)
        {
            if(strcmp(name,netdev_now->name)==0)return netdev_now->id;
            else if(netdev_now->next!=NULL) netdev_now=netdev_now->next;
            else return 0;
        }
    }else{
        return 0;
    }
}

int8_t netdev_set_name(netdev_id_t netdev_id,str * name)
{
    if(name_legality(name)<=0)return 0;
    XLnetdev * netdev=netdev_get(netdev_id);
    if(netdev_get_byname(name)<=0)
    {
        return 0;
    }else{
        strcpy(netdev->name,name);
        return 1;
    }
}


int8_t netdev_set_net(netdev_id_t id,ip ip,uint16_t port,uint8_t family)
{
    XLnetdev * netdev=netdev_get(id);
    if(netdev==NULL)return 0;
    netdev->net.ip=ip;
    netdev->net.family=family;
    netdev->net.port=port;
    return 1;
}
/*relate*/
int8_t relate_set(id_t dev1,dev_type_t type1,id_t dev2,dev_type_t type2,mod_t hard,mod_t soft)
{
    if(type1==DEV_TYPE_LO){if(dev_get(dev1)==NULL)return 0;}
    else if(type1==DEV_TYPE_NET){if(netdev_get(dev1)==NULL)return 0;}
    else return 0;
    if(type2==DEV_TYPE_LO){if(dev_get(dev2)==NULL)return 0;}
    else if(type2==DEV_TYPE_NET){if(netdev_get(dev2)==NULL)return 0;}
    else return 0;

    if(type1==type2)
    {
        if(type1==DEV_TYPE_LO)if(dev_get(dev1)==dev_get(dev2))return 0;
        if(type1==DEV_TYPE_NET)if(netdev_get(dev1)==netdev_get(dev2))return 0;
    }
    if(hard>2||hard<0)return 0;
    if(soft>2||soft<0)return 0;

    extern XLrelate * relate_head;
    XLrelate *relate_now=relate_head,*relate_new=NULL;
    if(relate_head==NULL)
    {
        relate_new=malloc(sizeof(struct XLrelate));
        memset(relate_new,0,sizeof(XLrelate));
        relate_head=relate_new;
    }
    else{
        int a=1;
        while(a!=0){
            if(relate_now->id1==dev1&&relate_now->dev1_type==type1&&relate_now->id2==dev2&&relate_now->dev2_type==type2){
                relate_new=relate_now;
                a=0;
            }
            else if(relate_now->next==NULL)
            {
                relate_new=malloc(sizeof(struct XLrelate));
                memset(relate_new,0,sizeof(XLrelate));
                relate_now->next=relate_new;
                a=0;
            }
            relate_now=relate_now->next;
        }
        //if(a==0)relate_new=relate_now;
        /*else if(a==2) {relate_new=malloc(sizeof(struct XLrelate));
        relate_now->next=relate_new;}*/
    }
    relate_new->id1=dev1;
    relate_new->id2=dev2;
    relate_new->dev1_type=type1;
    relate_new->dev2_type=type2;
    relate_new->hard=hard;
    relate_new->soft=soft;
    return 1;
}
int8_t relate_del(id_t dev1,dev_type_t type1,id_t dev2,dev_type_t type2)
{
    extern XLrelate * relate_head;
    XLrelate * relate_front=NULL,*relate_now=relate_head;
    while(relate_now!=NULL)
    {
        if(relate_now->id1==dev1&&relate_now->dev1_type==type1&&relate_now->id2==dev2&&relate_now->dev2_type==type2)
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
    return 0;
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
        printf("%d ",relate_now->id1);
        printf("(");
        if(relate_now->hard==RELATE_HARD_1p)printf("H");
        if(relate_now->soft==RELATE_SOFT_1p)printf("S");
        printf(")");
        if(relate_now->dev2_type==DEV_TYPE_LO)printf("LO:");
        if(relate_now->dev2_type==DEV_TYPE_NET)printf("NET:");
        printf("%d ",relate_now->id2);
        printf("(");
        if(relate_now->hard==RELATE_HARD_2p)printf("H");
        if(relate_now->soft==RELATE_SOFT_2p)printf("S");
        printf(")");
        printf("\n");
        relate_now=relate_now->next;
    }
}
