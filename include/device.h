#ifndef DEVICE_H
#define DEVICE_H

#include <malloc.h>
#include <type.h>
#include <signal.h>
#include<stdio.h>
#include<stdlib.h>


#define RELATE_HARD_NULL 0
#define RELATE_HARD_1p 1
#define RELATE_HARD_2p 2

#define RELATE_SOFT_NULL 0
#define RELATE_SOFT_1p 1
#define RELATE_SOFT_2p 2


#define NETWORK_IPV4 1
#define NETWORK_IPV6 2
#define NETWORK_BOTH 3

#define DEV_TYPE_LO 0
#define DEV_TYPE_NET 1

stack<int> i;
typedef struct XLnet {  //网络
    uint64_t net_ipv4;
    uint32_t net_ipv6[4];
    uint16_t port;
    uint8_t mode;
    struct XLnet * next;
} XLnet;

typedef struct XLrelate {
    dev_op_t op1;             //op1 	设备1
    uint8_t dev1_type;        //lo net	设备1的类型
	
    dev_op_t op2;             //op2	设备2
    uint8_t dev2_type;        //lo net	设备2的类型

    uint8_t hard;            //hard	设备1与设备2的硬件关系
    uint8_t soft;            //soft	设备1与设备2的软件关系

    struct XLrelate * next;
} XLrelate;


typedef struct XLdevice {       //设备结构体

    /*name*/
    str name[DEVICE_NAME_LENGTH];       //设备名
    /*network*/
    struct XLnet net;
    /*signal*/
    struct XLdevice_sig_list * signal_h;    //包含信号的信号表头
    /*op*/
    dev_op_t op;
    struct XLdevice * next_op;
} XLdevice;

typedef struct XLnetdev {               //网络设备
    /*name*/
    str name[DEVICE_NAME_LENGTH];       //设备名
    /*network*/
    struct XLnet net;
    /*op*/
    netdev_op_t op;
    struct XLnetdev * next_op;
} XLnetdev;

typedef struct XLdevice_sig_list {          //信号表
	struct XLsignal * signal;
    struct XLdevice_sig_list * next;
} XLdevice_sig_list;
/*device*/
dev_op_t device_create(void);                   //设备创建
dev_op_t device_del(dev_op_t dev_op);           //设备删除
XLdevice * device_get(dev_op_t dev_op);         //获得设备操作点
void device_list(void);                         //设备列表(测试用)
int8_t device_init(dev_op_t dev_op,str * name,dev_type_t type); //设备初始化
int8_t device_set_name(dev_op_t dev_op,str * name); //设备命名
sig_op_t device_get_signal(dev_op_t dev_op,str *name);  //设备通过名字获取对应信号操作点

/*netdevice*/
XLnetdev * netdev_get(netdev_op_t netdev_op);
netdev_op_t netdev_create(void);
netdev_op_t netdev_del(netdev_op_t dev_op);
void netdev_list(void) ;

/*relate*/
int8_t relate_set(op_t dev1,dev_type_t type1,op_t dev2,dev_type_t type2,mod_t hard,mod_t soft);
void relate_list(void);
#endif // DEVICE_H
