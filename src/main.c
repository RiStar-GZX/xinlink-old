/**
 * Copyright (C), 2022-2023, github.com/Ristar-GZX.
 * File name: main.c     // 文件名
 * Author:Ristar-GZX  Version:V0.0.1    Date:2023.1.5 // 作者、版本及完成日期
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
    /*
        //设备创建操作测试
       int i,op[num];
        for(i=0; i<=num-1; i++) {
                op[i]=netdev_create();

        }
        while(1) {
                int s=0;
                printf("\ndelete:");
                scanf("%d",&s);
                if(s==0)netdev_create();
                else netdev_del(s);
                netdev_list();
        }
*/

/*
        dev_op_t dev_op;
        sig_op_t sig_op;
        dev_op=device_create();
        sig_op=signal_create();
        //if(signal_set_dev(sig_op,dev_op)>0)
        if(device_init(dev_op,"test_device",DEVICE_TYPE_SN)>=0) {
                printf("sig_set OK!\n");
        } else printf("sig_set error!\n");
*/
    /*int dev=device_create();
        int sig=signal_create();
        device_set_name(dev,"dev1");
        signal_set_dev(sig,dev);
        signal_set_name(sig,"test");

        signal_set_slot(sig,my_sig_slot);

        signal_set_name(sig,"test114514");
        signal_slot(sig);
        //signal_get(sig)->fun(sig);
        //signal_set_slot(sig,signal_slot);
        //signal_send(sig);
        //signal_trigger(dev,"test");  
    */
//------------------------------------------------------------//
    /*test2  */
    /*for(int i=1;i<=10;i++)
    {
        device_create();
        netdev_create();
    }
    //dev_op_t n=0;
    while (1) {
        int s;
        printf("add(1),relate(2),show(3):");
        scanf("%d",&s);
        if(s==1)
        {
            int b;
            printf("lo(1),net(2):");
            scanf("%d",&b);
            if(b==1)device_create();
            if(b==2)netdev_create();
            //n++;
        }else if(s==2)
        {
            int dev1,type1,dev2,type2,hard,soft;
            printf("dev1,type1,dev2,type2,hard,soft:");
            scanf("%d %d %d %d %d %d",&dev1,&type1,&dev2,&type2,&hard,&soft);
            if(relate_set(dev1,type1,dev2,type2,hard,soft)<=0)printf("relate_set error!\n");
        }
        else if(s==3)
        {
            relate_list();
        }
    }*/
//------------------------------------------------------------//


    dev_op_t dev1=device_create();
    sig_op_t sig1=signal_create();
    device_set_name(dev1,"dev1");
    signal_set_name(sig1,"sig1");
    signal_set_dev(sig1,dev1);
    signal_add_par(sig1,"size");
    par_data_t a=549654;
    sigpar_set_data(sig1,"size",&a);
    signal_set_slot(sig1,my_sig_slot);
    //signal_send(dev1,"sig1");
    signal_slot(sig1);
    return 0;
}

