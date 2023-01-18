/**
 * Copyright (C), 2022-2023, github.com/RiStar-GZX.
 * File name: main.c     // 文件名
 * Author:Ristar-GZX  Version:V0.0.1    Date:2023.1.5 
 * Description:    // 用于详细说明此程序文件完成的主要功能，与其他模块
                   // 或函数的接口，输出值、取值范围、含义及参数间的控
                   // 制、顺序、独立或依赖等关系
 * Others:         // 其它内容的说明
 * Function List:  // 主要函数列表，每条记录应包括函数名及功能简要说明
    1. main
 * History:        // 修改历史记录列表，每条修改记录应包括修改日期、修改
                  // 者及修改内容简述
    1. Date:
       Author:
       Modification:
 */
#include <stdio.h>
#include <xinlink.h>
#define num 10


int my_sig_slot(sig_op_t sig_op)
{
    //printf("114514%d",sig_op);
    par_data_t * size;
    size=signal_get_par(sig_op,"size");
    printf("this is signal slot!\nsignal_name is:%s\npar(size):%d",signal_get_name(sig_op),*size);
    return 0;
}

int main() {

    net_init();
/*
    netdev_op_t netdevop=netdev_create();
    netdev_set_name(netdevop,"netdev55455");
    ip ip;
    ip.net_ipv4=inet_addr("192.168.1.11");
    netdev_set_net(netdevop,ip,6666,NETWORK_IPV4);
    //netdev_set_net(netdevop,)
    XLsig_pak pak;
    strcpy(pak.name,"signal");
    uint32_t a=0x34333231;
    //uint32_t a=0x04030201;
    uint8_t *d=&a;
    pak_add_par(&pak,"par1");
    pak_set_data(&pak,"par1",d,4);
    pak_add_par(&pak,"par5");
    net_send_pak(netdevop,&pak);*/
    while(1);
    return 0;
}

