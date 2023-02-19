/**
 * Cidyright (C), 2022-2023, github.com/RiStar-GZX.
 * File name: main.c     
 * Author:Ristar-GZX  Version:V0.0.1    Date:2023.1.28 
 * Description:    
      这个示例程序是在linux下通过xdotool来控制鼠标移动，这里是被控端
 */
#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/input.h>
#include <fcntl.h>
#include <xinlink.h>
#define num 10

/*
int my_slot(sig_id_t sig_id,XLpak * pak)
{
        //printf("\n\n\nThis is sig solt!\n\n\n");
        uint32_t datasize1,* data1=NULL;
        data1=pak_get_data(pak,"x",&datasize1);
        uint32_t datasize2,* data2=NULL;
        data2=pak_get_data(pak,"y",&datasize2);
        char s[50];
        sprintf(s,"xdotool mousemove %d %d\n",*data1,*data2);
        system(s);
        system("clear");
        return 1;
}
*/
int main() {
    /*net_init();
    dev_id_t dev=dev_create("device");
    sig_id_t sig=sig_create("signal");
    dev_set_sig(dev,sig);
    sig_set_slot(sig,my_slot);
    while (1);*/XLnet net;
    net.family=NETWORK_IPV4;
    net.ip.net_ipv4=inet_addr("192.168.1.23");
    printf("ip is:%x\n",net.ip.net_ipv4);
    net.port=8181;
    //while (1) {
    int a;
    scanf("%d",&a);
    if(a==1)net_search(&net);
    else net_rev_search();
        //sleep(1);
    //}
    return 0;
}
