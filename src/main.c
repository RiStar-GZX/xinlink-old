#include <stdio.h>
#include <core.h>
#include <event.h>
#include <network.h>
#include <instruction.h>
#include <string.h>

INS * yuanshen(mon_id_t mon_id){
    printf("\n\n原神 启动!\n\n");

    XLpak_ins * pak_ins=monitor_get_member(mon_id);
    while(1)
    {
        if(pak_ins!=NULL)if(pak_ins->ins!=NULL)printf("INS:%s\n",pak_ins->ins);
    monitor_remove_member(mon_id);
    sleep(1);
    }
}

INS * mouse(mon_id_t mon_id){
    printf("mounse start\n");
    int mode=0;
    XLpak_ins * pak_ins=monitor_get_member(mon_id);
    XLsource sender;
    if(pak_ins!=NULL&&pak_ins->ins!=NULL){
        if(strcmp(pak_ins->ins,"server")==0)//对方为服务端(被操控端）
        mode=1;
        if(strcmp(pak_ins->ins,"client")==0)//对方为客户端(操控端）
        mode=2;
        sender=pak_ins->sender;
    }
    else return NULL;
    monitor_remove_member(mon_id);
    if(mode==2)printf("server start\n");

    while(mode!=0)
    {
        pak_ins=monitor_get_member(mon_id);
        if(pak_ins!=NULL){
            if(mode==1){
                //network_ins_send(&sender,)
            }
            if(mode==2){
                int x,y;
                printf("ins:%s\n",pak_ins->ins);
                XLins_decoded * decode=ins_decode(pak_ins->ins);
                printf("argc:%d\n",decode->argc);
                if(decode->argc>=2){
                    x=atoi(decode->argv[0]);
                    y=atoi(decode->argv[1]);
                    char s[50];
                    sprintf(s,"xdotool mousemove %d %d\n",x,y);
                    system(s);
                    system("clear");
                }
            }
        }
        monitor_remove_member(mon_id);
        sleep(1);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    xinlink_init();
    app_add("yuanshen",yuanshen);
    app_add("mouse",mouse);
    printf("Xinlink - version(0.0.1)\n");
    while(1)
    {
        char *a=malloc(sizeof(str)*100);
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
            INS s[100];
            printf("core id:");
            scanf("%d",&id);
            printf("APP name:");
            scanf("%s",s);
            XLsource receiver,sender;
            receiver.name=s;
            receiver.mode=RECEIVER_START_APP;
            sender.mode=ACCESS;
            if(network_ins(&sender,&receiver,id,"client")>=0)printf("Success!\n");

            //if(network_ins_send(id,s)>=0)printf("Success!\n");
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
            XLsource receiver,sender;
            receiver.mode=RECEIVER_EVENT_ID;
            receiver.id=eid;
            sender.mode=ACCESS;
            str s[100];
            sprintf(s,"%d %d",x,y);
            if(network_ins(&sender,&receiver,id,s)>=0)printf("Success!\n");
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


/*
int main(int argc, char *argv[])
{
    INS ins[]="     event                  ls                        cool   s                    ";
    XLins_decoded * decoded=ins_decode(ins);
    if(decoded==NULL)printf("NO\n");
    else printf("YES\n\n");
    printf("argc:%d\n",decoded->argc);
    printf("argv:%s\n",decoded->argv[0]);

    printf("argv:%s\n",decoded->argv[1]);

    printf("argv:%s\n",decoded->argv[2]);

    printf("argv:%s\n",decoded->argv[3]);

    return 0;
}
*/
