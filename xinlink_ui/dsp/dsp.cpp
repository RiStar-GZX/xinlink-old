#include <stdio.h>
#include <dsp.h>
#include <core.h>
#include <vnc/vnc.h>
#include <curls/curls.h>
#include <curl/curl.h>

int dsp_init(void){
    app_add("vncclient",vncclient_init,vncclient,vncclient_exit);
    app_add("vnchost",vnchost_init,vnchost,vnchost_exit);
    app_add("curl",NULL,curl,NULL);
    event_create_run_add_sign("vnchost","vnchost","vnc",SIGN_CS_SERVER);
    event_create_run_add_sign("vncclient","vncclient","vnc",SIGN_CS_CLIENT);
    event_create_run_add_sign("curl","curl","curl",SIGN_CS_BOTH);
    return 1;
}
