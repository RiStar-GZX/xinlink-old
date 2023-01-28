/**
 * Cidyright (C), 2022-2023, github.com/RiStar-GZX.
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
    dev_id_t id1;             //id1 	设备1
    uint8_t dev1_type;        //lo net	设备1的类型
	
    dev_id_t id2;             //id2	设备2
    uint8_t dev2_type;        //lo net	设备2的类型

    uint8_t hard;            //hard	设备1与设备2的硬件关系
    uint8_t soft;            //soft	设备1与设备2的软件关系

    struct XLrelate * next;
} XLrelate;


typedef struct XLdev {       //设备结构体

    /*name*/
    str name[DEVICE_NAME_LENGTH];       //设备名
    /*network*/
    struct XLnet net;
    /*sig*/
    struct XLdev_sig_list * sig_h;    //包含信号的信号表头
    /*id*/
    dev_id_t id;
    struct XLdev * next_id;
} XLdev;

typedef struct XLnetdev {               //网络设备
    /*name*/
    str name[DEVICE_NAME_LENGTH];       //设备名
    /*network*/
    struct XLnet net;
    /*id*/
    netdev_id_t id;
    struct XLnetdev * next_id;
} XLnetdev;

typedef struct XLdev_sig_list {          //信号表
    struct XLsig * sig;
    struct XLdev_sig_list * next;
} XLdev_sig_list;


int8_t name_legality(str * name);
/*dev*/

dev_id_t dev_create(str * name);

dev_id_t dev_del(dev_id_t dev_id);

XLdev * dev_get(dev_id_t dev_id);

void dev_list(void);

int8_t dev_init(dev_id_t dev_id,str * name,dev_type_t type);

int8_t dev_set_name(dev_id_t dev_id,str * name);
int8_t dev_set_sig(dev_id_t dev_id,sig_id_t sig_id);
sig_id_t dev_get_sig(dev_id_t dev_id,str *name);  //设备通过名字获取对应信号操作点

dev_id_t dev_get_byname(str * name);
/*网络设备部分*/
netdev_id_t netdev_create(str *name);

XLnetdev * netdev_get(netdev_id_t netdev_id);

netdev_id_t netdev_del(netdev_id_t dev_id);
void netdev_list(void) ;
netdev_id_t netdev_get_byname(str * name);
int8_t netdev_set_name(netdev_id_t netdev_id,str * name);
int8_t netdev_set_net(netdev_id_t id,ip ip,uint16_t port,uint8_t family);
/*relate*/
int8_t relate_set(id_t dev1,dev_type_t type1,id_t dev2,dev_type_t type2,mod_t hard,mod_t soft);
void relate_list(void);


#endif // dev_H
