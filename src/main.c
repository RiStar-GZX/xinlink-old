#include <stdio.h>
#include <core.h>
#include <event.h>
#include <network.h>
#include <instruction.h>
#include <string.h>
/*
INS * mouse_server(XLevent_par * par){
    XLsource *sender,receiver;
    //sender=monitor_get_receiver(par->mon_id);
    monitor_remove_member(par->mon_id);
    XLpak_ins * pak_ins;
    while(1){
        if((pak_ins=monitor_get_member(par->id))==NULL){
            monitor_remove_member(par->mon_id);
            sleep(1);
            continue;
        }
        if(ins_get_par(pak_ins->ins,"connect")==1){
            printf("start new connection\n");
            receiver=pak_ins->sender;
        }
        else if(source_cmp(&receiver,&pak_ins->sender)){
            int x,y;
            printf("ins:%s\n",pak_ins->ins);
            x=ins_get_par_int(pak_ins->ins,"x",0);
            y=ins_get_par_int(pak_ins->ins,"y",0);
            char s[50];
            sprintf(s,"xdotool mousemove %d %d\n",x,y);
            system(s);
        }
        monitor_remove_member(par->mon_id);
    }
}

INS * mouse(XLevent_par * par){
    printf("mouse start!\n");
    int mode=0,j=1;
    XLpak_ins * pak_ins=monitor_get_member(par->mon_id);
    XLins_decoded *decode;
    XLsource *sender,*receiver,sed;
    if(pak_ins!=NULL&&pak_ins->ins!=NULL){
        decode=ins_decode(pak_ins->ins);
        if(strcmp(decode->argv[0],"client")==0)//作为客户端(操控端） 2
        mode=1;
        if(strcmp(decode->argv[0],"server")==0)//作为服务端(被操控端）1
        mode=2;
    }
    else return NULL;

    if(ins_get_par_ip(pak_ins->ins,"-ip",&sed.net.ip)<0)j=0;
    if(sed.net.port=ins_get_par_int(pak_ins->ins,"-port",-1)==-1)j=0;
    sed.mode=EVENT_ID;
    if(sed.id=ins_get_par_int(pak_ins->ins,"-eid",-1)==-1)j=0;
    if(j==1)mode+=2;

    monitor_remove_member(par->mon_id);
    printf("mode:%d\n",mode);
    if(mode==3){
        receiver=&sed;
        network_ins(&pak_ins->receiver,&receiver,"server");
        //while(1){
          //  if((pak_ins=monitor_get_member(par->mon_id))==NULL)break;
            printf("dsdsadsdad\n");
        //}
        mode=1;
    }
    while (mode==1) {
        system("xdotool getmouselocation > a.txt");
        FILE * file;
        file=fopen("./a.txt","rb+");
        fseek(file,2,SEEK_SET);
        int x=0,y=0,t;
        while((t=getc(file))!=32)x=x*10+(t-48);
        fseek(file,2,SEEK_CUR);
        while((t=getc(file))!=32)y=y*10+(t-48);
        char s[10];
        sprintf(s,"x %d y %d",x,y);
        fclose(file);
        printf("%s\n",s);
        network_ins(&sender,&receiver,s);
    }
    if(mode==2)printf("server start\n");

    while(mode==2){
        pak_ins=monitor_get_member(par->mon_id);
        if(pak_ins!=NULL){
            int x,y;
            printf("ins:%s\n",pak_ins->ins);
            decode=ins_decode(pak_ins->ins);
            printf("argc:%d\n",decode->argc);
            x=ins_get_par_int(pak_ins->ins,"x",0);
            y=ins_get_par_int(pak_ins->ins,"y",0);
            char s[50];
            sprintf(s,"xdotool mousemove %d %d\n",x,y);
            system(s);
        }
        monitor_remove_member(par->mon_id);
        sleep(1);
    }
    return NULL;
}



INS * mouse_client(XLevent_par * par){

}
*/

INS * test(XLevent_par * par){
    //monitor_get_source(par->mon_id);
    printf("test start\n");
    while(1){
        XLpak_ins * pak_ins=NULL;
        pak_ins=monitor_get_member(par->mon_id);

        if(pak_ins!=NULL)
        {
            printf("is:%s\n",pak_ins->ins);
            if(ins_get_par(pak_ins->ins,"connect")){
                XLsource source;
                source.mode=EVENT_ID;
                source.id=1;
                source.net=network_get_local_info();
                monitor_limit_add_source(par->mon_id,&source);
            }
        }

        monitor_remove_member(par->mon_id);
        sleep(1);
    }
}


int main(int argc, char *argv[])
{
    xinlink_init();
    //app_add("mouse",mouse_server);
    //app_add("mousec",mouse_client);
    app_add("test",test);
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
            core_list();
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
            scanf("%s",ins);
            printf("ins:%s\n",ins);
            XLcore *core;
            if((core=core_get_by_id(id))==NULL)continue;
            XLsource receiver,sender;
            receiver.name=s;
            receiver.net=core->net;
            receiver.mode=RECEIVER_START_APP;
            sender.mode=ACCESS;
            if(network_ins(&sender,&receiver,ins)>=0)printf("Success!\n");
            else printf("Fail!\n");
        }
        else if(strcmp(a,"insse")==0){
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
            receiver.mode=RECEIVER_EVENT_ID;
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
            receiver.mode=RECEIVER_EVENT_ID;
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
            receiver.mode=RECEIVER_EVENT_ID;
            receiver.id=eid;
            receiver.net=core->net;
            sender.mode=ACCESS;
            sender.id=11;
            char s[10];
            sprintf(s,"x %d y %d",x,y);
            if(network_ins(&sender,&receiver,s)>=0)printf("Success!\n");
            else printf("Fail!\n");
        }
        else if(strcmp(a,"showapp")==0){
            app_list();
        }
        else if(strcmp(a,"showevent")==0){
            event_show();
        }
        else {
            printf("help           寻求帮助\n");
            printf("findcore       寻找其他核心\n");
            printf("showcore       展示其他核心\n");
            printf("connectcore    与其他核心对接\n");
            printf("disconnect     断开与其他核心的连接\n");
            printf("inss           向指定核心发送启动指令\n");
            printf("showapp        展示应用\n");
            printf("showevent      展示事件\n");
            printf("inse           向指定事件发指令\n");
        }
    }
    return 0;
}
