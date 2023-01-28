/**
 * Copyright (C), 2022-2023, github.com/RiStar-GZX.
 * File name: main.c     // 文件名
 * Author:Ristar-GZX  Version:V0.0.1    Date:2023.1.28
 * Description:
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <xinlink.h>
#define num 10

int main() {
    netdev_id_t netdev;
    netdev=netdev_create("device");
    XLsig_pak pak;
    pak.sig_par_h=NULL;
    union ip ip;
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
        printf("x:%d y:%d\n",x,y);
        pak_set_data(&pak,"x",&x,sizeof(x));
        pak_set_data(&pak,"y",&y,sizeof(y));
        fclose(file);
        system("clear");
        net_send_pak(netdev,&pak);
    }
    return 0;
}

