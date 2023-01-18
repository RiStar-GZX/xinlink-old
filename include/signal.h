#ifndef SIGNAL_H
#define SIGNAL_H
#include <type.h>
#include<device.h>
#include <malloc.h>
#include<stdio.h>
#include<stdlib.h>
#define SIGNAL_NAME_LENGTH 64

typedef  int (*FUNC)(sig_op_t sig_op);

typedef struct XLsignal_par {       //信号参数结构体
        str name[PAR_NAME_LENGTH];          //参数名
        uint8_t * data;                 //参数数据
        uint32_t datasize;
        struct XLsignal_par * next;     //下一个参数
} XLsignal_par;



typedef struct XLsignal {           //信号

        sig_op_t op;                    //信号操作点
        struct XLsignal * next_op;      //下一个信号
        struct XLdevice * device;           //归属设备
        struct XLsignal_par * signal_par_h; //参数，为空时表示没有参数
        str  name[SIGNAL_NAME_LENGTH];  //名字
        /*SLOT*/
        FUNC fun;
        //SIG_SLOT fun;
} XLsignal;

typedef struct XLsig_pak{
    str name[SIGNAL_NAME_LENGTH];
    struct XLsignal_par * signal_par_h;
    dev_op_t op;
    //struct XLnet net;
}XLsig_pak;


XLsignal * signal_get(sig_op_t op);     //获得信号的结构体
sig_op_t signal_create(void);           //创建信号
sig_op_t signal_del(sig_op_t sig_op);   //删除信号

int8_t signal_set_dev(sig_op_t sig_op,dev_op_t dev_op);     //设置信号归属设备
int8_t signal_set_name(sig_op_t sig_op,str * str);          //设置信号的名字(在一个设备中唯一)
str * signal_get_name(sig_op_t sig_op);						//获得信号名字
dev_op_t signal_get_dev(sig_op_t sig_op);					//信号操作点获取设备操作点

void signal_list(void) ;

int8_t signal_add_par(sig_op_t sig_op,str * name);			//信号设置参数
int8_t signal_del_par(sig_op_t sig_op,str * name);			//信号删除参数

int8_t signal_send(dev_op_t dev_op,str * sig_name);      //触发信号
int8_t signal_set_slot(sig_op_t sig_op,FUNC slot);          //设置信号槽
int8_t signal_slot(sig_op_t sig_op);                        //启动信号槽
par_data_t * signal_get_par(sig_op_t sig_op,str * name);
int8_t sigpar_set_data(sig_op_t sig_op,str * name,par_data_t * data);

int8_t pak_del_par(XLsig_pak *  pak,str * name);
int8_t pak_add_par(XLsig_pak * pak,str * name);
int8_t pak_set_data(XLsig_pak * pak,str * name,uint8_t * data,uint32_t size);
#endif // SIGNAL_H

