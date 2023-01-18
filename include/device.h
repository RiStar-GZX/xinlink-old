/**
 * Copyright (C), 2022-2023, github.com/RiStar-GZX.
 * File name: device.h
 * Author:GZX  Version:V0.0.1    Date:2023.1.5
 * 描述:
 输出设备创建和操作的函数
 返回rs:
 * 函数 List:
    1. main
 * History:
    1. Date:
       Author:
       Modification:
*/
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
//#define NETWORK_BOTH 3

#define NETWORK_IPV4 1
#define NETWORK_IPV6 2

#define DEV_TYPE_LO 0
#define DEV_TYPE_NET 1

/**
    
 * 函数:       // 函数名称
 * 描述:    // 函数功能、性能等的描述
 * 输出:          // 被本函数调用的函数清单
 * 返回 By:      // 调用本函数的函数清单
 * Table Accessed: // 被访问的表(此项仅对于牵扯到数据库操作的程序)
 * Table Updated:  // 被修改的表(此项仅对于牵扯到数据库操作的程序)
 * Input:          // 输入参数说明，包括每个参数的作
                  // 用、取值说明及参数间关系。
 * Output:         // 对输出参数的说明。
 * Return:         // 函数返回值的说明
 * 其他s:         // 其它说明
 */
typedef union ip{
    uint32_t net_ipv4;
    uint8_t net_ipv6[6];
}ip;

typedef struct XLnet {  //网络
    union ip ip;
    uint8_t family;
    uint16_t port;
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

/*设备部分*/
/** 
 * 函数: device_create
 * 描述: 创建一个空设备并且返回申请的设备的操作点 
 * 输出: 空
 * 返回: 成功返回申请的操作点，失败返回-1 
 */
dev_op_t device_create(void);       

/** 
 * 函数: device_del
 * 描述: 删除指定设备 
 * 输出: 要删除设备的操作点
 * 返回: 成功返回1，失败返回-1 
 */
dev_op_t device_del(dev_op_t dev_op);    

/** 
 * 函数: device_get
 * 描述: 通过设备操作点获得设备的结构体
 * 输入: 设备的操作点
 * 返回: 成功返回设备的结构体指针，失败返回NULL
 * 其他: 一般不要在应用程序中调用 
 */
XLdevice * device_get(dev_op_t dev_op);         

/** 
 * 函数: device_list
 * 其他: 测试用 
 */
void device_list(void); 

/** 
 * 函数: device_init
 * 描述: 设备初始化
 * 输出: 设备的操作点、要初始化设备的名称
 * 返回: 成功返回1，失败返回-1 
 */                 
int8_t device_init(dev_op_t dev_op,str * name,dev_type_t type);

/** 
 * 函数: device_set_name
 * 描述: 设备设置名字
 * 输入: 设备操作点、设备设置名字
 * 返回: 成功返回1，失败返回-1 
 */  
int8_t device_set_name(dev_op_t dev_op,str * name); 

/** 
 * 函数: device_get_signal
 * 描述: 获得设备中信号的操作点
 * 输入: 设备操作点、信号名
 * 返回: 成功返回信号从操作点，失败返回0 
 */  
sig_op_t device_get_signal(dev_op_t dev_op,str *name);  //设备通过名字获取对应信号操作点

/*网络设备部分*/
/** 
 * 函数: netdev_create
 * 描述: 创建一个空网络设备并且返回申请的网络设备的操作点 
 * 输出: 空
 * 返回: 成功返回申请的操作点，失败返回-1 
 */
netdev_op_t netdev_create(void);
/** 
 * 函数: netdev_get
 * 描述: 通过网络设备操作点获得网络设备的结构体
 * 输入: 网络设备的操作点
 * 返回: 成功返回网络设备的结构体指针，失败返回NULL
 * 其他: 一般不要在应用程序中调用 
 */
XLnetdev * netdev_get(netdev_op_t netdev_op);

netdev_op_t netdev_del(netdev_op_t dev_op);
void netdev_list(void) ;
netdev_op_t netdev_get_byname(str * name);
int8_t netdev_set_name(netdev_op_t netdev_op,str * name);
int8_t netdev_set_net(netdev_op_t op,ip ip,uint16_t port,uint8_t family);
/*relate*/
int8_t relate_set(op_t dev1,dev_type_t type1,op_t dev2,dev_type_t type2,mod_t hard,mod_t soft);
void relate_list(void);


#endif // DEVICE_H
