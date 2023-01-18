#include <signal.h>

XLsignal * signal_head=NULL;            //信号头

XLsignal * signal_get(sig_op_t op) {    //获得信号结构体
        extern XLsignal * signal_head;      //信号头
        XLsignal * signal_now=signal_head;  //操作指针指向头
        for(;;) {
                if(signal_now->op==op)
                        return signal_now;
                else if(signal_now->next_op!=NULL)
                        signal_now=signal_now->next_op;
                else return NULL;
        }
}

sig_op_t signal_create(void) {                  //信号创建
        XLsignal * signal_new=malloc(sizeof(XLsignal));     //新建信号内存
        extern XLsignal * signal_head;                      //导入信号头

        if(signal_head==NULL) {                             //如果没有信号
                signal_head=signal_new;                         //新建第一个信号
                signal_new->op=1;                               //操作点为1
                signal_new->next_op=NULL;                       //无下一位
                return signal_new->op;
        } else if(signal_head->op>1) {                      //如果信号头的操作点没有占到1
                signal_new->op=1;                               //新建信号操作点为1
                XLsignal * temp;
                temp=signal_head;
                signal_head=signal_new;
                signal_new->next_op=temp;
                return signal_new->op;
        } else {                                            //其他情况
                XLsignal * signal_now;

                signal_now=signal_head;

                for(;;) {
                        if(signal_now->next_op==NULL)  {            //如果没有空位

                                signal_now->next_op=signal_new;
                                signal_new->op=signal_now->op+1;
                                return signal_new->op;;
                        } else if(signal_now->op+1<signal_now->next_op->op) { //如果下一个设备的操作符与当前设备操作符有空位

                                signal_new->op=signal_now->op+1;            //新设备占据这个操作符

                                signal_new->next_op=signal_now->next_op;       //设置新设备的下一个设备

                                signal_now->next_op=signal_new;             //设置现在设备的下一个设备为新设备

                                return signal_new->op;;
                        } else  signal_now=signal_now->next_op;  //下一个

                }

        }
}
sig_op_t signal_del(sig_op_t sig_op) {
        extern XLsignal * signal_head;              //导入设备头
        XLsignal * signal_now=signal_head;          //操作指针指向头
        XLsignal * signal_nowfront=NULL;            //记录操作指针前一个
        for(;;) {
                if(signal_now->op==sig_op) {            //如果有这个设备
                        if(signal_now==signal_head) {           //如果这个设备是设备头
                                if(signal_head->next_op==NULL) {        //如果就剩这一个设备
                                        //free(signal_head);                    //释放内存
                                        signal_head=NULL;                       //释放指针
                                        return 1;                                 //成功返回
                                } else {                                //如果还有其他设备
                                        XLsignal * temp;
                                        temp=signal_head->next_op;              //将设备头赋予下一个设备
                                        //free(signal_head);                    //释放内存
                                        signal_head=temp;
                                        return 1;;                                 //成功返回
                                }
                        } else {                                //如果这个设备不是头
                                if(signal_now->next_op==NULL) {         //如果这个设备后面没有设备了
                                        //free(signal_now);                       //释放内存
                                        signal_nowfront->next_op=NULL;          //删除前面一个设备的后端指针
                                        return 1;                                //成功返回
                                } else {                                //如果没有设备了
                                        XLsignal * temp;
                                        temp=signal_now->next_op;
                                        signal_nowfront->next_op=temp;          //前面一个设备的后端指针指向后一个设备
                                        //free(signal_now);                       //释放内存
                                        return 1;                                 //成功返回
                                }
                        }
                } else if(signal_now->next_op!=NULL) {          //如果不是要找的设备但后面还有设备
                        signal_nowfront=signal_now;                     //记录下当前操作设备的前一个设备
                        signal_now=signal_now->next_op;                 //当前操作设备向后移动一个
                } else return -1;                               //如果既不是要找的设备后面也没有设备了，报错退出
        }
}

void signal_list(void) {
        extern XLsignal * signal_head;
        XLsignal * signal_now=signal_head;
        int i=0;
        while(i==0) {

                printf(" %d",signal_now->op);
                if(signal_now->next_op==NULL)i=1;
                else signal_now=signal_now->next_op;
        }
}


int8_t signal_set_dev(sig_op_t sig_op,dev_op_t dev_op) {    //设置信号归属设备
        XLdevice * device=device_get(dev_op);                   //获得设备结构体
        XLsignal * signal=signal_get(sig_op);                   //获得信号结构体
        if(signal!=NULL&&device!=NULL&&strcmp(signal->name,"")!=0) {    //判断设备、信号、信号名三者非空
                XLdevice_sig_list *dev_sig_now=device->signal_h;        //设备信号表的操作指针
                if(dev_sig_now!=NULL)                                   //当前设备有信号
                        for(;;) {                                               //循环找出信号
                                if(dev_sig_now->signal->name==signal->name)return -1;
                                else if(dev_sig_now->next!=NULL)dev_sig_now=dev_sig_now->next; //移位下一个
                                else {                                                          //读完了
                                        signal->device=device;                                      //接到设备的信号表尾
                                        dev_sig_now->next=malloc(sizeof(XLdevice_sig_list));
                                        dev_sig_now->next->signal=signal;
                                        dev_sig_now->next->signal->device=device;
                                        dev_sig_now->next->next=NULL;
                                        return 1;
                                }
                        }
                else {
                        signal->device=device;
                        dev_sig_now=malloc(sizeof(XLdevice_sig_list));
                        dev_sig_now->signal=signal;
                        dev_sig_now->signal->device=device;
                        dev_sig_now->next=NULL;
                }
                return 1;
        } else return -1;
}
dev_op_t signal_get_dev(sig_op_t sig_op) {
        XLsignal * signal=signal_get(sig_op);
        if(signal!=NULL)
        {
            if(signal->device->op>0)return signal_get(sig_op)->device->op;
        }
        return 0;
}



int8_t signal_set_name(sig_op_t sig_op,str * str) {     //设置信号的名字
        XLsignal *signal=signal_get(sig_op);                //通过操作点获得信号结构体
        if(str==NULL)return -1;
        if(signal->device==NULL) {
                strcpy(signal->name,str);//设置信号的名字
                return 1;
        } else {
                if(signal->device->signal_h!=NULL) {                //如果当前设备有信号
                        XLdevice_sig_list * dev_sig_now=signal->device->signal_h;   //获得当前信号位于的设备的信号表头
                        for(;;) {                                               //循环判断是否有重名
                                if(dev_sig_now->signal->name==signal->name)return -1;      //重名报错返回-1
                                else if(dev_sig_now->next!=NULL)dev_sig_now=dev_sig_now->next;  //如果未到表底当前指针向下移位
                                else {
                                        strcpy(signal->name,str);//设置信号的名字
                                        return 1;
                                }                                               //无重名退出循环
                        }
                } else {
                        strcpy(signal->name,str);//设置信号的名字
                        return 1;
                }
        }
}

str * signal_get_name(sig_op_t sig_op) {     //信号的名字
        return signal_get(sig_op)->name;
}


int8_t signal_add_par(sig_op_t sig_op,str * name) {			//添加信号参数（信号操作点，参数名）
        XLsignal * sig=signal_get(sig_op);
        if(sig->signal_par_h!=NULL) {							//信号有参数
                XLsignal_par * par_now=sig->signal_par_h;
                for(;;) {
                        if(name==par_now->name)return -1;
                        else if(par_now->next!=NULL)par_now=par_now->next;
                        else {
                                XLsignal_par * par_new=malloc(sizeof(XLsignal_par));
                                par_new->next=NULL;
                                strcpy(par_new->name,name);
                                par_now->next=par_new;
                                return 1;
                        }
                }
        } else {													//信号没参数
                XLsignal_par * par_new=malloc(sizeof(XLsignal_par));	//接入参数表头
                par_new->next=NULL;
                strcpy(par_new->name,name);
                sig->signal_par_h=par_new;
                return 1;
        }
}

int8_t signal_del_par(sig_op_t sig_op,str * name) {			//删除信号函数（信号操作点，信号名）
        XLsignal * sig=signal_get(sig_op);
        if(sig->signal_par_h!=NULL) {
                XLsignal_par * par_now=sig->signal_par_h;
                XLsignal_par * par_now_front=NULL;
                for(;;) {
                        if(strcmp(name,par_now->name)==0) {						//名字一样
                                if(par_now==sig->signal_par_h) {							//位于表头
                                        if(par_now->next==NULL) {							//无下一个
                                                free(par_now);
                                                return 1;
                                        } else {											//有下一个
                                                sig->signal_par_h=par_now->next;				//下一个设为表头
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
                        } else return -1;

                }
        } else return -1;
}

par_data_t * signal_get_par(sig_op_t sig_op,str * name)
{
    XLsignal * sig=signal_get(sig_op);
    XLsignal_par * sigpar_now=sig->signal_par_h;
    if(sigpar_now!=NULL)
    {
        while(sigpar_now!=NULL&&strcmp(sigpar_now->name,name)!=0)
        {
            sigpar_now=sigpar_now->next;
        }
        if(strcmp(sigpar_now->name,name)==0){
            par_data_t * data=&sigpar_now->data;
            return data;
        }
        else return NULL;
    }
    else return NULL;
}

int8_t sigpar_set_data(sig_op_t sig_op,str * name,par_data_t * data)
{
    XLsignal * sig=signal_get(sig_op);
    XLsignal_par * sigpar_now=sig->signal_par_h;
    if(sigpar_now!=NULL)
    {
        while(sigpar_now!=NULL&&strcmp(sigpar_now->name,name)!=0)
        {
            sigpar_now=sigpar_now->next;
        }
        if(strcmp(sigpar_now->name,name)==0){
            sigpar_now->data=data;
        }
        else return -1;
    }
    return -1;
}

int8_t signal_send(dev_op_t dev_op,str * sig_name)
{
    if(device_get(dev_op)!=NULL)
    {
        XLsignal *signal;
        signal=signal_get(device_get_signal(dev_op,sig_name));

        if(signal!=NULL)
        {
            signal->fun(signal->op);
            return 1;
        }
    }
    return -1;
}

int8_t signal_slot(sig_op_t sig_op)
{
    XLsignal *signal;
    signal=signal_get(sig_op);

    if(signal!=NULL)
    {
        signal->fun(sig_op);
        return 0;
    }
    else return -1;
}


int8_t signal_set_slot(sig_op_t sig_op,FUNC slot)
{
    XLsignal * signal=signal_get(sig_op);
    if(signal!=NULL)
    {
        signal->fun=slot;
    }
    return -1;
}


int8_t pak_add_par(XLsig_pak * pak,str * name) {			//添加信号参数（信号操作点，参数名）

        if(pak->signal_par_h!=NULL) {							//信号有参数
                XLsignal_par * par_now=pak->signal_par_h;
                for(;;) {
                        if(name==par_now->name)return -1;
                        else if(par_now->next!=NULL)par_now=par_now->next;
                        else {
                                XLsignal_par * par_new=malloc(sizeof(XLsignal_par));
                                par_new->next=NULL;
                                strcpy(par_new->name,name);
                                par_now->next=par_new;
                                return 1;
                        }
                }
        } else {													//信号没参数
                XLsignal_par * par_new=malloc(sizeof(XLsignal_par));	//接入参数表头
                par_new->next=NULL;
                strcpy(par_new->name,name);
                pak->signal_par_h=par_new;
                return 1;
        }
}

int8_t pak_del_par(XLsig_pak *  pak,str * name) {			//删除信号函数（信号操作点，信号名）
        if(pak->signal_par_h!=NULL) {
                XLsignal_par * par_now=pak->signal_par_h;
                XLsignal_par * par_now_front=NULL;
                for(;;) {
                        if(strcmp(name,par_now->name)==0) {						//名字一样
                                if(par_now==pak->signal_par_h) {							//位于表头
                                        if(par_now->next==NULL) {							//无下一个
                                                free(par_now);
                                                return 1;
                                        } else {											//有下一个
                                                pak->signal_par_h=par_now->next;				//下一个设为表头
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
                        } else return -1;

                }
        } else return -1;
}

int8_t pak_set_data(XLsig_pak * pak,str * name,uint8_t * data,uint32_t size)
{
    XLsignal_par * par_now=pak->signal_par_h;
    if(par_now!=NULL)
    {
        while(par_now!=NULL&&strcmp(par_now->name,name)!=0)
        {
            par_now=par_now->next;
        }
        if(strcmp(par_now->name,name)==0){
            par_now->data=data;
            par_now->datasize=size;
        }
        else return -1;
    }
    return -1;
}

