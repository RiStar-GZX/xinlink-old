#include <stdio.h>
#include <event.h>
#include <network.h>

#include <instruction.h>
#include <core.h>
#include <string.h>
/*
INS * FFmpeg_server(XLevent_par * par){
    printf("ffmpeg start!");
    monitor_remove_all_member(par->mon_id);
    while(1){
        monitor_get_member(par->id);
    }
}

INS * FFplay_client(XLevent_par * par){
    printf("ffplay start!\n");
}

INS * mouse_server(XLevent_par * par){
    printf("mouse server start!\n");
    monitor_remove_member(par->mon_id);
    mon_id_t mon_ac=0;
    XLpak_ins * pak_ins,*ac_ins;
    while(1){
        ac_ins=monitor_get_member(mon_ac);
        if(ac_ins!=NULL){
            int x,y;
            printf("ins:%s\n",ac_ins->ins);
            ins_get_par_int(ac_ins->ins,"x",&x);
            ins_get_par_int(ac_ins->ins,"y",&y);
            char s[100];
            sprintf(s,"xdotool mousemove %d %d\n",x,y);
            system(s);
        }
        pak_ins=monitor_get_member(par->id);
        if(pak_ins!=NULL&&ins_get_par(pak_ins->ins,"connect")==1){
            printf("start new connection\n");
            if(mon_ac==0)mon_ac=monitor_create(monitor_get_source(par->mon_id));
            monitor_limit_add_source(mon_ac,&pak_ins->sender);
        }
        monitor_remove_member(par->mon_id);
        monitor_remove_member(mon_ac);
        if(monitor_get_member(par->id)!=NULL)continue;
        usleep(1000);
    }
}
*/
/*
INS * mouse_client(XLevent_par * par){
    XLpak_ins * pak_ins;
    pak_ins=monitor_get_member(par->mon_id);
    XLsource source;
    if(ins_get_par_ip(pak_ins->ins,"ip",&source.net.ip)<=0)return NULL;
    if(!ins_get_par_int(pak_ins->ins,"port",(int*)&source.net.port))return NULL;

    if(!ins_get_par_int(pak_ins->ins,"ID",&source.id))return NULL;
    if(ins_get_par(pak_ins->ins,"EID")){
        source.mode=EVENT_ID;
    }
    printf("ip:%x  port:%d  id:%d\n",source.net.ip,source.net.port,source.id);

    printf("mouse client start\n");

    network_ins(monitor_get_source(par->mon_id),&source,"connect");
    int X=100,Y=100;
    while(1){
        system("xdotool getmouselocation > a.txt");
        FILE * file;
        file=fopen("./a.txt","rb+");
        fseek(file,2,SEEK_SET);
        int x,y,t;
        while((t=getc(file))!=32)x=x*10+(t-48);
        fseek(file,2,SEEK_CUR);
        while((t=getc(file))!=32)y=y*10+(t-48);
        //char s[50]="x 100 y 100";

        char s[50];
       if(X==1000){
            X=100;
            if(Y>=1000)Y=100;
            else Y+=4;
        }
        X+=4;

        //fclose(file);
        if(x==X&&y==Y){
            usleep(1000);
            continue;
        }
        //printf("%s\n",s);
        sprintf(s,"x %d y %d",X,Y);
        //X=x;
        //Y=y;
        network_ins(monitor_get_source(par->mon_id),&source,s);
        usleep(10000);
    }
}
*/

INS * vnchost(XLevent_par * par){
    printf("vnchost start!\n");
    popen("x0vncserver -display :0 -passwordfile ~/.vnc/passwd","r");
    monitor_remove_member(par->mon_id);
    while(1){
        XLpak_ins * pak_ins=monitor_get_member(par->mon_id);
        if(pak_ins==NULL){
            usleep(10000);
            continue;
        }
        //if(pak_ins->sender.mode==CORE_MYSELF&&ins_get_par(pak_ins->ins,"exit")){
        //    return NULL;
        //}
        if(ins_get_par(pak_ins->ins,"connectwith")){
            //printf(" connectwith source 192.168.1.12:8081:SN:vncclient");
            printf("get ins\n");
            //system("neofetch");
            XLsource * receiver=ins_get_par_source(pak_ins->ins,"source");

            if(receiver!=NULL){
                network_ins(monitor_get_source(par->mon_id),receiver,"connect -f -h");
                printf("OKOKOK!\n");
            }
        }
        monitor_remove_member(par->mon_id);
    }
    return NULL;
}

INS * vncclient(XLevent_par * par){
    printf("vncclient start!\n");
    while(1)
    {
        XLpak_ins * pak_ins=monitor_get_member(par->mon_id);
        if(pak_ins==NULL){
            usleep(10000);
            continue;
        }
        printf("have\n");
        if(ins_get_par(pak_ins->ins,"connect")){
            char str[100];
            struct in_addr ip;
            ip.s_addr=pak_ins->sender.net.ip;
            sprintf(str,"remmina -c vnc://%s:5900",inet_ntoa(ip));
            if(ins_get_par(pak_ins->ins,"-f")>0)strcat(str," --enable-fullscreen");
            if(ins_get_par(pak_ins->ins,"-h")>0)strcat(str," --enable-extra-hardening");
            printf("str:%s\n",str);
            system(str);
        }
        monitor_remove_member(par->mon_id);
    }
}

INS * test(XLevent_par * par){
    printf("test start\n");
    while(1){
        XLpak_ins * pak_ins=NULL;
        pak_ins=monitor_get_member(par->mon_id);

        if(pak_ins!=NULL)
        {
            printf("[%s(%s)]is:%s\n",par->sign->name,par->sign->type,pak_ins->ins);
            if(ins_get_par(pak_ins->ins,"connect")){
                XLsource source;
                source.mode=EVENT_ID;
                source.id=1;
                source.net=network_get_local_info();
                monitor_limit_add_source(par->mon_id,&source);
            }
            if(ins_get_par(pak_ins->ins,"disconnect")){
                monitor_limit_add_source(par->mon_id,NULL);
            }
        }

        monitor_remove_member(par->mon_id);
        sleep(1);
    }
}

INS * keyborad_host(XLevent_par * par){
    printf("keyborad host start\n");
    mon_id_t mo=0;
    while(1){
        XLpak_ins * pak_ins=NULL;

        pak_ins=monitor_get_member(mo);
        char stmp[50];
        if(pak_ins!=NULL){
            int key=0;
            //char c[16]="123A456B789C*0#D";
            char c[16]="qwerasdftm9C1234";
            char * str=NULL;
            str=ins_get_par_str(pak_ins->ins,"key");
            if(str!=NULL){
                char s[50];
                printf("s;%s\n",str);
                for(int i=0;i<16;i++){
                    if(str[i]=='1'&&stmp[i]!=str[i]){
                        printf("i:%d %c \n",i,str[i]);
                        sprintf(s,"xdotool keydown %c",c[i]);
                        system(s);
                    }

                    if(str[i]=='0'&&stmp[i]!=str[i]){
                        sprintf(s,"xdotool keyup %c",c[i]);
                        system(s);
                    }
                }
                strcpy(stmp,str);
            }
        }
        pak_ins=monitor_get_member(par->mon_id);
        if(pak_ins!=NULL)
        {
            if(ins_get_par(pak_ins->ins,"connect")){
                printf("connectwith\n\n\n");
                XLsource * source;
                source=ins_get_par_source(pak_ins->ins,"source");
                if(source!=NULL){
                    network_ins(monitor_get_source(par->id),source,"connect");
                    mo=monitor_create(monitor_get_source(par->id));
                    monitor_limit_add_source(mo,source);
                    //printf("IP:%x port:%d mode:%d id:%d",source->net.ip,source->net.port,source->mode,source->id);
                    //printf("name:%s",source->name);
                    printf("OKOKOK\n");
                }
            }
            if(ins_get_par(pak_ins->ins,"disconnect")){
                XLsource * source;
                source=ins_get_par_source(pak_ins->ins,"source");
                //if(source_cmp(source,mo))
                //network_ins()
            }
        }
        if(mo>0)monitor_remove_member(mo);
        monitor_remove_member(par->mon_id);
        usleep(10000);
    }
}

int main(int argc, char *argv[])
{
    xinlink_init();
    printf("app:%d\n",app_add("vncclient",vncclient));
    printf("app:%d\n",app_add("vnchost",vnchost));

    event_id_t client_id=event_create_and_run("vncclient");
    event_id_t host_id=event_create_and_run("vnchost");
    event_add_sign(host_id,"vnchost","vnc");
    event_add_sign(client_id,"vncclient","vnc");
    event_show();

    XLpak_sign pak_sign;
    printf("Xinlink - version(0.0.1)\n");
    while(1)
    {
        char *a=malloc(sizeof(char)*100);
        printf(">");
        scanf("%s",a);
        if(strcmp(a,"findcore")==0){
            network_core_find_send();
        }
        else if(strcmp(a,"showcore")==0){
            core_show();
        }
        else if(strcmp(a,"connectcore")==0){
            int id;
            printf("core id:");
            scanf("%d",&id);
            if(network_core_connect_require_send(id)>=0)printf("Success!\n");
            else printf("Fail!\n");
        }
        else if(strcmp(a,"disconnect")==0){
            int id;
            printf("core id:");
            scanf("%d",&id);
            if(network_core_disconnect_send(id)>=0)printf("Success!\n");
            else printf("Fail!\n");
        }
        else if(strcmp(a,"disconnect")==0){
            int id;
            printf("core id:");
            scanf("%d",&id);
            if(network_core_disconnect_send(id)>=0)printf("Success!\n");
            else printf("Fail!\n");
        }
        else if(strcmp(a,"inss")==0){
            int id;
            char s[100];
            char ins[100];
            printf("core id:");
            scanf("%d",&id);
            printf("APP name:");
            scanf("%s",s);
            printf("INS:");
            scanf("%[^\n]",ins);
            printf("ins:%s\n",ins);
            XLcore *core;
            if((core=core_get_by_id(id))==NULL)continue;
            XLsource receiver,sender;
            receiver.name=s;
            receiver.net=core->net;
            receiver.mode=START_APP;
            sender.mode=ACCESS;
            if(network_ins(&sender,&receiver,ins)>=0)printf("Success!\n");
            else printf("Fail!\n");
        }
        else if(strcmp(a,"insse")==0){
            int id,eid;
            char ins[200];
            printf("core id:");
            scanf("%d",&id);
            printf("event id:");
            scanf("%d",&eid);
            printf("ins:");
            scanf("%[^\n]",ins);
            //gets(ins);
            printf("ins:%s\n",ins);
            XLcore *core;
            if((core=core_get_by_id(id))==NULL)continue;
            XLsource receiver,sender;
            receiver.mode=EVENT_ID;
            receiver.id=eid;
            receiver.net=core->net;
            sender.mode=ACCESS;
            sender.id=11;
            if(network_ins(&sender,&receiver,ins)>=0)printf("Success!\n");
            else printf("Fail!\n");
        }
        else if(strcmp(a,"insnb")==0){
            int id,eid;
            char ins[100];
            printf("core id:");
            scanf("%d",&id);
            printf("event id:");
            scanf("%d",&eid);
            printf("ins:");
            scanf("%s",ins);
            XLcore *core;
            if((core=core_get_by_id(id))==NULL)continue;
            XLsource receiver,sender;
            receiver.mode=EVENT_ID;
            receiver.id=eid;
            receiver.net=core->net;
            sender.mode=EVENT_ID;
            sender.id=1;
            if(network_ins(&sender,&receiver,ins)>=0)printf("Success!\n");
            else printf("Fail!\n");
        }
        else if(strcmp(a,"inse")==0){
            int id,eid;
            int x,y;
            printf("core id:");
            scanf("%d",&id);
            printf("event id:");
            scanf("%d",&eid);
            printf("pos:");
            scanf("%d %d",&x,&y);
            XLcore *core;
            if((core=core_get_by_id(id))==NULL)continue;
            XLsource receiver,sender;
            receiver.mode=EVENT_ID;
            receiver.id=eid;
            receiver.net=core->net;
            sender.mode=ACCESS;
            sender.id=11;
            char s[10];
            sprintf(s,"x %d y %d",x,y);
            if(network_ins(&sender,&receiver,s)>=0)printf("Success!\n");
            else printf("Fail!\n");
        }
        else if(strcmp(a,"inssi")==0){
            int id;
            char name[100],s[100];
            printf("core id:");
            scanf("%d",&id);
            printf("sign name:");
            scanf("%s",&name);
            printf("ins:");
            scanf("%s",s);
            XLcore *core;
            if((core=core_get_by_id(id))==NULL)continue;
            XLsource receiver,sender;
            receiver.mode=SIGN_NAME;
            receiver.name=name;
            receiver.net=core->net;
            sender.mode=ACCESS;
            sender.id=11;
            if(network_ins(&sender,&receiver,s)>=0)printf("Success!\n");
            else printf("Fail!\n");
        }
        else if(strcmp(a,"showapp")==0){
            app_show();
        }
        else if(strcmp(a,"showevent")==0){
            event_show();
        }
        else if(strcmp(a,"sendsign")==0){
            int b;
            scanf("%d",&b);
            if(network_send_sign(b)>=0){
                printf("Success!\n");
                //core_get_sign(b);
            }
            else printf("Fail\n");
        }
        else if(strcmp(a,"getsign")==0){
            int b;
            scanf("%d",&b);
            if(network_require_sign(b)>=0){
                printf("Success!\n");
                //core_get_sign(b);
            }
            else printf("Fail\n");
        }
        else {
            printf("help(寻求帮助)          ");
            printf("findcore(寻找核心)      ");
            printf("showcore(展示核心)      ");
            printf("connectcore(与核心对接)        ");
            printf("disconnect(断开与核心的连接)    ");
            printf("showapp(展示应用)       ");
            printf("showevent(展示事件)     ");
            printf("getsign(获得设备标识符)  ");
            printf("inss(启动应用指令)       ");
            printf("inse(向指定事件发指令)  \n");
        }
        usleep(10000);
    }
    return 0;
}

/*
int main(int argc ,char **argv){

    char ret[1024] = {0};
    FILE *fp;

    //FILE *popen(const char *command, const char *type);
    //fp = popen("x0vncserver","r");
    fp = popen("ls -a","r");

    //int nread = fread(ret,1,1024,fp);
    //printf("read ret %d byte, %s\n",nread,ret);
    while(1){
        printf("S\n");
        sleep(1);
        //system("neofetch");

    }
    return 0;
}*/

/*
#include <ll.h>

INS * test(XLevent_par * par){
    return NULL;
}
int main(){

    int num=0;
    /*XLll * ll=ll_create(sizeof(XLnet));
    ll_add_member_tail(ll,&num,sizeof(XLnet));
    num=1;
    ll_add_member_tail(ll,&num,sizeof(XLnet));
    num=2;
    ll_add_member_tail(ll,&num,sizeof(XLnet));
    num=3;
    ll_add_member_tail(ll,&num,sizeof(XLnet));

    ll_del_member_tail(ll);
    ll_del_member_tail(ll);
    ll_del_member_tail(ll);

    ll_del_member_head(ll);
    ll_del_member_head(ll);
    ll_del_member_head(ll);

//    ll_del_member_head(ll);


    //num=1;
    ll_show_member(ll);
    while(1){
        int a;
        scanf("%d",&a);
        if(a==0)event_create(test);
        else event_remove(a);
        extern XLll *event_ll;
        ll_show_member(event_ll);
        event_show();
    }
    return 0;
}
*/
