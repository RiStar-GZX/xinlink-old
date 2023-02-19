#include <signal.h>

XLsig * sig_head=NULL;            //信号头

XLsig * sig_get(sig_id_t id) {    //获得信号结构体
        extern XLsig * sig_head;      //信号头
        XLsig * sig_now=sig_head;  //操作指针指向头
        for(;;) {
                if(sig_now->id==id)
                        return sig_now;
                else if(sig_now->next_id!=NULL)
                        sig_now=sig_now->next_id;
                else return NULL;
        }
}
sig_id_t sig_create(str * name) {                  //信号创建
        if(name_legality(name)<=0)return 0;        //名字合法性判断
        XLsig * sig_new=malloc(sizeof(XLsig));     //新建信号内存
        memset(sig_new,0,sizeof(XLsig));
        extern XLsig * sig_head;                      //导入信号头

        if(sig_head==NULL) {                             //如果没有信号
                sig_head=sig_new;                         //新建第一个信号
                sig_new->id=1;                               //操作点为1
                sig_new->next_id=NULL;                       //无下一位
                strcpy(sig_new->name,name);
                return sig_new->id;
        } else if(sig_head->id>1) {                      //如果信号头的操作点没有占到1
                sig_new->id=1;                               //新建信号操作点为1
                XLsig * temp;
                temp=sig_head;
                sig_head=sig_new;
                sig_new->next_id=temp;
                strcpy(sig_new->name,name);
                return sig_new->id;
        } else {                                            //其他情况
                XLsig * sig_now;

                sig_now=sig_head;

                for(;;) {
                        if(sig_now->next_id==NULL)  {            //如果没有空位

                                sig_now->next_id=sig_new;
                                sig_new->id=sig_now->id+1;
                                strcpy(sig_new->name,name);
                                return sig_new->id;;
                        } else if(sig_now->id+1<sig_now->next_id->id) { //如果下一个设备的操作符与当前设备操作符有空位

                                sig_new->id=sig_now->id+1;            //新设备占据这个操作符

                                sig_new->next_id=sig_now->next_id;       //设置新设备的下一个设备

                                sig_now->next_id=sig_new;             //设置现在设备的下一个设备为新设备
                                strcpy(sig_new->name,name);
                                return sig_new->id;;
                        } else  sig_now=sig_now->next_id;  //下一个

                }

        }
}
sig_id_t sig_del(sig_id_t sig_id) {
        extern XLsig * sig_head;              //导入设备头
        XLsig * sig_now=sig_head;          //操作指针指向头
        XLsig * sig_nowfront=NULL;            //记录操作指针前一个
        for(;;)
        {
                if(sig_now->id==sig_id) //如果有这个设备
                {
                        if(sig_now==sig_head) //如果这个设备是设备头
                        {
                                if(sig_head->next_id==NULL) //如果就剩这一个设备
                                {
                                    free(sig_head);                      //释放内存
                                    sig_head=NULL;                       //释放指针
                                    return 1;                               //成功返回
                                }
                                else                //如果还有其他设备
                                {
                                    XLsig * temp;
                                    temp=sig_head->next_id;              //将设备头赋予下一个设备
                                    free(sig_head);                      //释放内存
                                    sig_head=temp;
                                    return 1;                               //成功返回
                                }
                        }
                        else                                //如果这个设备不是头
                        {
                            if(sig_now->next_id==NULL)           //如果这个设备后面没有设备了
                            {
                                free(sig_now);                       //释放内存
                                sig_nowfront->next_id=NULL;          //删除前面一个设备的后端指针
                                return 1;                               //成功返回
                            }
                            else                            //如果没有设备了
                            {
                                XLsig * temp;
                                temp=sig_now->next_id;
                                sig_nowfront->next_id=temp;          //前面一个设备的后端指针指向后一个设备
                                free(sig_now);                       //释放内存
                                return 1;                               //成功返回
                            }
                        }
                }
                else if(sig_now->next_id!=NULL)          //如果不是要找的设备但后面还有设备
                {
                        sig_nowfront=sig_now;                     //记录下当前操作设备的前一个设备
                        sig_now=sig_now->next_id;                 //当前操作设备向后移动一个
                }
                else return 0;                             //如果既不是要找的设备后面也没有设备了，报错退出
        }
}

void sig_list(void) {
        extern XLsig * sig_head;
        XLsig * sig_now=sig_head;
        int i=0;
        while(i==0) {

                printf(" %d",sig_now->id);
                if(sig_now->next_id==NULL)i=1;
                else sig_now=sig_now->next_id;
        }
}

dev_id_t sig_get_dev(sig_id_t sig_id) {
        XLsig * sig=sig_get(sig_id);
        if(sig!=NULL)
        {
            if(sig->dev->id>0)return sig_get(sig_id)->dev->id;
        }
        return 0;
}



int8_t sig_set_name(sig_id_t sig_id,str * name) {     //设置信号的名字
        if(name_legality(name)<=0)return 0;
        XLsig *sig=sig_get(sig_id);                //通过操作点获得信号结构体
        if(sig->dev==NULL) {
                strcpy(sig->name,name);//设置信号的名字
                return 1;
        } else {
            if(sig->dev->sig_h!=NULL) {                //如果当前设备有信号
                XLdev_sig_list * dev_sig_now=sig->dev->sig_h;   //获得当前信号位于的设备的信号表头
                for(;;) {                                               //循环判断是否有重名
                        if(dev_sig_now->sig->name==sig->name)return 0;      //重名报错返回-1
                        else if(dev_sig_now->next!=NULL)dev_sig_now=dev_sig_now->next;  //如果未到表底当前指针向下移位
                        else {
                                strcpy(sig->name,name);//设置信号的名字
                                return 1;
                        }                                                      //无重名退出循环
                    }
                } else {
                        strcpy(sig->name,name);//设置信号的名字
                        return 1;
                }
        }
}

str * sig_get_name(sig_id_t sig_id) {     //信号的名字
        return sig_get(sig_id)->name;
}


int8_t sig_add_par(sig_id_t sig_id,str * name) {			//添加信号参数（信号操作点，参数名）
        XLsig * sig=sig_get(sig_id);
        if(sig->sig_par_h!=NULL) {							//信号有参数
                XLsig_par * par_now=sig->sig_par_h;
                for(;;) {
                        if(name==par_now->name)return 0;
                        else if(par_now->next!=NULL)par_now=par_now->next;
                        else {
                                XLsig_par * par_new=malloc(sizeof(XLsig_par));
                                memset(par_new,0,sizeof(XLsig_par));
                                par_new->next=NULL;
                                strcpy(par_new->name,name);
                                par_now->next=par_new;
                                return 1;
                        }
                }
        } else {													//信号没参数
                XLsig_par * par_new=malloc(sizeof(XLsig_par));	//接入参数表头
                memset(par_new,0,sizeof(XLsig_par));
                par_new->next=NULL;
                strcpy(par_new->name,name);
                sig->sig_par_h=par_new;
                return 1;
        }
}

int8_t sig_del_par(sig_id_t sig_id,str * name) {			//删除信号函数（信号操作点，信号名）
        XLsig * sig=sig_get(sig_id);
        if(sig->sig_par_h!=NULL) {
                XLsig_par * par_now=sig->sig_par_h;
                XLsig_par * par_now_front=NULL;
                for(;;) {
                        if(strcmp(name,par_now->name)==0) {						//名字一样
                                if(par_now==sig->sig_par_h) {							//位于表头
                                        if(par_now->next==NULL) {							//无下一个
                                                free(par_now);
                                                return 1;
                                        } else {											//有下一个
                                                sig->sig_par_h=par_now->next;				//下一个设为表头
                                                free(par_now);
                                                return 1;
                                        }
                                } else if(par_now->next==NULL) {						//不是头且没有下一个
                                        par_now_front->next=NULL;							//前一个的下一个指空
                                        free(par_now);
                                        return 1;
                                } else {												//不是头但有下一个
                                        par_now_front->next=par_now->next;					//前后相连
                                        free(par_now);
                                        return 1;
                                }
                        } else if(par_now->next!=NULL) {							//
                                par_now_front=par_now;
                                par_now=par_now->next;
                        } else return 0;

                }
        } else return 0;
}

uint8_t * sigpar_get_data(sig_id_t sig_id,str * name,uint32_t * datasize)
{
    XLsig * sig=sig_get(sig_id);
    XLsig_par * sigpar_now=sig->sig_par_h;
    if(sigpar_now!=NULL)
    {
        while(sigpar_now!=NULL&&strcmp(sigpar_now->name,name)!=0)
        {
            sigpar_now=sigpar_now->next;
        }
        if(strcmp(sigpar_now->name,name)==0)
        {
            *datasize=sigpar_now->datasize;
            return sigpar_now->data;
        }
    }
    return NULL;
}

int8_t sigpar_set_data(sig_id_t sig_id,str * name,par_data_t * data,uint32_t datasize)
{
    XLsig * sig=sig_get(sig_id);
    XLsig_par * sigpar_now=sig->sig_par_h;
    if(sigpar_now!=NULL)
    {
        while(sigpar_now!=NULL&&strcmp(sigpar_now->name,name)!=0)
        {
            sigpar_now=sigpar_now->next;
        }
        if(strcmp(sigpar_now->name,name)==0){
            sigpar_now->data=data;
            sigpar_now->datasize=datasize;
            return 1;
        }
    }
    return 0;
}

int8_t sig_send(dev_id_t dev_id,str * sig_name,XLpak * pak)
{
    if(dev_get(dev_id)!=NULL)
    {
        if(sig_slot(dev_get_sig(dev_id,sig_name),pak)<=0)return 0;
        return 1;
    }
    else return 0;
}

int8_t sig_slot(sig_id_t sig_id,XLpak * pak)
{
    XLsig *sig;
    sig=sig_get(sig_id);

    if(sig!=NULL)
    {
        sig->fun(sig_id,pak);
        return 1;
    }
    else return 0;
}


int8_t sig_set_slot(sig_id_t sig_id,FUNC slot)
{
    XLsig * sig=sig_get(sig_id);
    if(sig!=NULL)
    {
        sig->fun=slot;
    }
    return 0;
}


int8_t pak_add_par(XLpak * pak,str * name) {			//添加信号参数（信号操作点，参数名）

        if(pak->sig_par_h!=NULL) {							//信号有参数
                XLsig_par * par_now=pak->sig_par_h;
                for(;;) {
                        if(name==par_now->name)return 0;
                        else if(par_now->next!=NULL)par_now=par_now->next;
                        else {
                                XLsig_par * par_new=malloc(sizeof(XLsig_par));
                                memset(par_new,0,sizeof(XLsig_par));
                                par_new->next=NULL;
                                strcpy(par_new->name,name);
                                par_now->next=par_new;
                                return 1;
                        }
                }
        } else {													//信号没参数
                XLsig_par * par_new=malloc(sizeof(XLsig_par));	//接入参数表头
                memset(par_new,0,sizeof(XLsig_par));
                par_new->next=NULL;
                strcpy(par_new->name,name);
                pak->sig_par_h=par_new;
                return 1;
        }
}

int8_t pak_del_par(XLpak *  pak,str * name) {			//删除信号函数（信号操作点，信号名）
        if(pak->sig_par_h!=NULL) {
                XLsig_par * par_now=pak->sig_par_h;
                XLsig_par * par_now_front=NULL;
                for(;;) {
                        if(strcmp(name,par_now->name)==0) {						//名字一样
                                if(par_now==pak->sig_par_h) {							//位于表头
                                        if(par_now->next==NULL) {							//无下一个
                                                free(par_now);
                                                return 1;
                                        } else {											//有下一个
                                                pak->sig_par_h=par_now->next;				//下一个设为表头
                                                free(par_now);
                                                return 1;
                                        }
                                } else if(par_now->next==NULL) {						//不是头且没有下一个
                                        par_now_front->next=NULL;							//前一个的下一个指空
                                        free(par_now);
                                        return 1;
                                } else {												//不是头但有下一个
                                        par_now_front->next=par_now->next;					//前后相连
                                        free(par_now);
                                        return 1;
                                }
                        } else if(par_now->next!=NULL) {							//
                                par_now_front=par_now;
                                par_now=par_now->next;
                        } else return 0;

                }
        } else return 0;
}

int8_t pak_set_data(XLpak * pak,str * name,uint8_t * data,uint32_t datasize)
{
    XLsig_par * par_now=pak->sig_par_h;
    if(par_now!=NULL)
    {
        while(par_now!=NULL&&strcmp(par_now->name,name)!=0)
        {
            par_now=par_now->next;
        }
        if(strcmp(par_now->name,name)==0){
            par_now->data=data;
            par_now->datasize=datasize;
        }
        else return 0;
    }
    return 0;
}

uint8_t * pak_get_data(XLpak * pak,str * name,uint32_t * datasize)
{
    if(pak==NULL||strcmp(name,"")==0)return 0;
    XLsig_par * sigpar_now=pak->sig_par_h;
    if(sigpar_now!=NULL)
    {
        while(sigpar_now!=NULL&&strcmp(sigpar_now->name,name)!=0)
        {
            sigpar_now=sigpar_now->next;
        }
        if(strcmp(sigpar_now->name,name)==0)
        {
            *datasize=sigpar_now->datasize;
            return sigpar_now->data;
        }
    }
    return 0;
}

