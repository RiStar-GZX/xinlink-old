#include <stdio.h>
#include <dsp.h>
#include <core.h>

/**********************************************/
/*              vnc host                      */
/**********************************************/
void * vnchost_init(XLevent_par * par){
    printf("vnchost start!\n");
    popen("x0vncserver -display :0 -passwordfile ~/.vnc/passwd","r");
    return NULL;
}

void * vnchost(XLevent_par * par){
    if(par->pak_ins==NULL)return NULL;

    if(ins_get_par(par->pak_ins->ins,(char*)"connectwith")){
        XLsource * receiver=ins_get_par_source(par->pak_ins->ins,(char*)"source");
        if(receiver!=NULL){
            network_send_ins(par->source,receiver,0,(char*)"connect -f -h");
        }
    }
    return NULL;
}

void * vnchost_exit(XLevent_par * par){
    printf("vnchost exit!\n");
    return NULL;
}

/**********************************************/
/*              vnc client                    */
/**********************************************/
void * vncclient_init(XLevent_par * par){
    printf("vncclient_init!\n");
    return NULL;
}

void * vncclient(XLevent_par * par){
    char str[100];
    struct in_addr ip;
    XLsource * source;
    if((source=ins_get_par_source(par->pak_ins->ins,"connectwith"))!=NULL){
        ip.s_addr=source->net.ip;
    }
    else if(ins_get_par(par->pak_ins->ins,"connect")==1){
        ip.s_addr=par->pak_ins->sender.net.ip;
    }
    sprintf(str,"remmina -c vnc://%s:5900",inet_ntoa(ip));
    if(ins_get_par(par->pak_ins->ins,"-f")>0)strcat(str," --enable-fullscreen");
    if(ins_get_par(par->pak_ins->ins,"-h")>0)strcat(str," --enable-extra-hardening");
    printf("str:%s\n",str);
    popen(str,"r");
    return NULL;
}

void * vncclient_exit(XLevent_par * par){
    printf("vncclient_exit!\n");
    return NULL;
}
