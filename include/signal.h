#ifndef SIGNAL_H
#define SIGNAL_H
#include <type.h>
#include<device.h>
#include <malloc.h>
#include<stdio.h>
#include<stdlib.h>
#define SIGNAL_NAME_LENGTH 64


typedef struct XLsig_par {       //信号参数结构体
        str name[PAR_NAME_LENGTH];          //参数名
        uint8_t * data;                 //参数数据
        uint32_t datasize;
        struct XLsig_par * next;     //下一个参数
} XLsig_par;

typedef struct XLsig_pak{
    str name[SIGNAL_NAME_LENGTH];
    mode_t mode;
    struct XLsig_par * sig_par_h;
    dev_id_t id;
    //struct XLnet net;
}XLsig_pak;

typedef  int (*FUNC)(sig_id_t sig_id,XLsig_pak * pak);


typedef struct XLsig {           //信号

        sig_id_t id;                    //信号操作点
        struct XLsig * next_id;      //下一个信号
        struct XLdev * dev;           //归属设备
        struct XLsig_par * sig_par_h; //参数，为空时表示没有参数
        str  name[SIGNAL_NAME_LENGTH];  //名字
        /*SLOT*/
        FUNC fun;
        //SIG_SLOT fun;
} XLsig;





XLsig * sig_get(sig_id_t id);     //获得信号的结构体
sig_id_t sig_get_byname(str * name);
sig_id_t sig_create(str * name);           //创建信号
sig_id_t sig_del(sig_id_t sig_id);   //删除信号

int sig_set_name(sig_id_t sig_id,str * str);          //设置信号的名字(在一个设备中唯一)
str * sig_get_name(sig_id_t sig_id);						//获得信号名字
dev_id_t sig_get_dev(sig_id_t sig_id);					//信号操作点获取设备操作点

void sig_list(void) ;

int sig_add_par(sig_id_t sig_id,str * name);			//信号设置参数
int sig_del_par(sig_id_t sig_id,str * name);			//信号删除参数

int sig_send(dev_id_t dev_id,str * sig_name,XLsig_pak * pak);     //触发信号
int sig_set_slot(sig_id_t sig_id,FUNC slot);          //设置信号槽
int sig_slot(sig_id_t sig_id,XLsig_pak * pak);                  //启动信号槽
uint8_t * sigpar_get_data(sig_id_t sig_id,str * name,uint32_t * datasize);
int sigpar_set_data(sig_id_t sig_id,str * name,par_data_t * data,uint32_t datasize);

int pak_del_par(XLsig_pak *  pak,str * name);
int pak_add_par(XLsig_pak * pak,str * name);
int pak_set_data(XLsig_pak * pak,str * name,void * data,uint32_t datasize);
uint8_t * pak_get_data(XLsig_pak * pak,str * name,uint32_t * datasize);
#endif // sig_H

