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
int main()
{
    net_init();
    dev_id_t dev=dev_create("device");
    sig_id_t sig=sig_create("signal");
    dev_set_sig(dev,sig);
    sig_set_slot(sig,my_slot);
    while (1);
    return 0;
}*/

/*int main() {
    netdev_id_t netdev;
    netdev=netdev_create("device");
    XLpak pak;
    pak.sig_par_h=NULL;
    struct ip ip;
    ip.net_ipv4=inet_addr("192.168.1.23");
    netdev_set_net(netdev,ip,8181,NETWORK_IPV4);
    strcpy(pak.name,"signal");
    pak_add_par(&pak,"x");
    pak_add_par(&pak,"y");
    while (1) {
        system("xdotool getmouselocation > a");
        FILE * file;
        file=fopen("./a","r+");
        fseek(file,2,SEEK_SET);
        int x=0,y=0,t;
        while((t=getc(file))!=32)x=x*10+(t-48);
        fseek(file,2,SEEK_CUR);
        while((t=getc(file))!=32)y=y*10+(t-48);
        system("clear");
        pak_set_data(&pak,"x",&x,sizeof(x));
        pak_set_data(&pak,"y",&y,sizeof(y));
        fclose(file);
        system("clear");
        net_send_pak(netdev,&pak);
    }
    return 0;
}*/

int main()
{
    dev_id_t dev=dev_create("device");
    netdev_id_t netdev=netdev_create("device");
    XLpak pak;
    strcpy(pak.name,"signal");
    pak_add_par(&pak,"x");
    pak_add_par(&pak,"y");
    int x=0x30313233;
    int y=x;
    pak_set_data(&pak,"x",&x,sizeof(x));
    pak_set_data(&pak,"y",&y,sizeof(y));
    ip ip;
    ip.net_ipv4=inet_addr("192.168.1.23");
    netdev_set_net(netdev,ip,8181,NETWORK_IPV4);
    while(1)
    {
        net_send_pak(netdev,&pak);
        sleep(1);
    }
    return 0;
}


