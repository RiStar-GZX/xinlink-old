/**
 * Cidyright (C), 2022-2023, github.com/RiStar-GZX.
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
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/input.h>
#include <fcntl.h>
#include <xinlink.h>
#define num 10


int my_slot(sig_id_t sig_id,XLsig_pak * pak)
{
        //printf("\n\n\nThis is sig solt!\n\n\n");
        uint32_t datasize1,* data1=NULL;
        data1=pak_get_data(pak,"x",&datasize1);
        uint32_t datasize2,* data2=NULL;
        data2=pak_get_data(pak,"y",&datasize2);
        uint32_t datasize3;uint8_t * data3=NULL;
        data3=pak_get_data(pak,"click",&datasize3);
        char s[50];
        sprintf(s,"xdotool mousemove %d %d\n",*data1,*data2);
        system(s);
        system("clear");
        printf("\n\n\n%d\n\n\n",*data3);
        /*if(data3>=10)sprintf(s,"xdotool mouseup %d %d\n",*data3-10);
        else sprintf(s,"xdotool mousedown %d %d\n",*data3);
        system(s);*/
        return 1;
}

void judge(int a,str * s)
{
    if(a<=0)printf("%s error\n",s);
}

int main() {
    dev_id_t dev=dev_create("device");
    sig_id_t sig=sig_create("signal");
    dev_set_sig(dev,sig);
    sig_set_slot(sig,my_slot);
    net_init();
    while (1);
    return 0;
}

