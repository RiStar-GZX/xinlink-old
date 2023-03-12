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


int my_slot(sig_id_t sig_id,XLsig_pak * pak)
{
        printf("\n\n\nThis is sig solt!\n\n\n");
        uint32_t datasize1,* data1=NULL;
        data1=pak_get_data(pak,"x",&datasize1);
        uint32_t datasize2,* data2=NULL;
        data2=pak_get_data(pak,"y",&datasize2);
        if(data1==NULL||data2==NULL)return 0;
        char s[50];
        sprintf(s,"xdotool mousemove %d %d\n",*data1,*data2);
        system(s);
        system("clear");
        return 1;
}
int main()
{
    xinlink_init();
    dev_id_t dev=dev_create("mouse");
    sig_id_t sig=sig_create("pos");
    dev_set_sig(dev,sig);
    sig_set_slot(sig,my_slot);
    while (1){
        int s;
        scanf("%d",&s);
        if(s==1)net_send_core();
        else if(s==2)net_send_dev_info(2);
        else if(s==3&&netdev_get_byname("mouse")!=0){
            netdev_id_t netdev=0;
            netdev=netdev_get_byname("mouse");
            if(netdev!=0)
            {
                XLsig_pak pak;
                pak.sig_par_h=NULL;
                strcpy(pak.name,"pos");
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
                    net_send_sig(netdev,&pak);
                }
            }
        }
        else printf("无法进入,没有网络设备\n");
   }
    return 0;
}
