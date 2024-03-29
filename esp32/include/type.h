#ifndef TYPE_H
#define TYPE_H

//config
//#define PLATFORM_LINUX
#define PLATFORM_ESP32

#define ENABLE   1
#define DISABLE  0

#define CORE_NAME_LENGTH 64
#define APP_NAME_LENGTH 64
#define DEV_NAME_LENGTH 64
#define APP_NAME_LENGTH 64
/* base */
#ifndef uint_8_t
typedef unsigned              char     uint8_t;
#endif

#ifndef uint_16_t
typedef unsigned    short     int      uint16_t;
#endif

#ifndef uint_32_t
typedef unsigned              int      uint32_t;
#endif

#ifndef uint
typedef unsigned              int      uint;
#endif


#ifndef int8_t
typedef               char     int8;
#endif

#ifndef int16_t
typedef    short      int      int16;
#endif

#ifndef int32_t
typedef               int      int32;
#endif
/*
#ifndef str
typedef char char;
#endif
*/
#ifndef core_id_t
typedef int core_id_t;
#endif

#ifndef dev_id_t
typedef int      dev_id_t;
#endif


#ifndef INS
typedef char INS;
#endif
#ifndef DATA
typedef uint8_t DATA;
#endif
#ifndef IP
typedef uint32_t IP;
#endif

#ifndef MAC
typedef uint8_t MAC;
#endif

#ifndef PORT
typedef uint16_t PORT;
#endif

#ifndef LEVEL
typedef uint8_t LEVEL;
#endif

#ifndef app_id_t
typedef unsigned int      app_id_t;
#endif

#ifndef event_id_t
typedef unsigned int      event_id_t;
#endif

#ifndef mon_id_t
typedef unsigned int      mon_id_t;
#endif

#ifndef sign_id_t
typedef unsigned int      sign_id_t;
#endif

//----------INS----------//

typedef  struct XLins_decoded_data{
    //str * data_name[64];
    uint8_t *data;
    int datasize;
    struct XLins_decoded_data * next;
}XLins_decoded_data;

typedef  struct XLins_decoded{
    //XLins_decoded_data * data_list;
    uint8_t ** argv;
    int argc;
}XLins_decoded;
//----------NET----------//
typedef struct XLnet {
    IP ip;
    MAC mac[6];
    PORT port;
}XLnet;

typedef struct XLsource{
    int mode;
    int id;
    XLnet net;
    char * name;
}XLsource;

typedef struct XLsource_list{
    XLsource source;
    struct XLsource_list * next;
}XLsource_list;


//----------NETWORK_PACKET----------//

typedef struct XLpak_base{
    uint16_t mode;
    uint16_t pak_size;
    XLnet net_sender;
    XLnet net_receiver;
}XLpak_base;

typedef struct XLpak_ins{
    XLpak_base base;
    XLsource sender;
    XLsource receiver;
    INS * ins;
}XLpak_ins;


typedef struct XLpak_connect{
    XLpak_base base;
    char * core_name;
}XLpak_connect;


typedef struct XLpak_signinfo{
    //uint8_t size; //名字和类型总和
    char * name;
    char * type;
    struct XLpak_signinfo * next;
}XLpak_signinfo;

typedef struct XLpak_sign{
    XLpak_base base;
    uint8_t sign_num;
    XLpak_signinfo * sign_list;
}XLpak_sign;


//----------QUEUE----------//

typedef union XLqueue_in{
    XLpak_ins pak_ins;
    XLpak_connect pak_connect;
    XLpak_sign pak_sign;
}XLqueue_in;

typedef struct XLqueue {
    int mode;
    XLqueue_in in;
    LEVEL level;
    struct XLqueue * front;
    struct XLqueue * next;
}XLqueue;

typedef struct XLqueue_head {
    XLqueue * queue;
}XLqueue_head;

typedef struct XLsign{
    uint8_t use; //是否使用设备标识符
    char * name;
    char * type;
    void * contrast;
    int contrast_size;
}XLsign;

typedef struct XLsign_list{
    XLsign sign;
    struct XLsign_list * next;
}XLsign_list;

//----------CORE----------//

typedef struct XLcore {
    char name[CORE_NAME_LENGTH];
    core_id_t id;
    XLnet net;
    uint8_t safety;
    XLpak_signinfo * sign_list;
}XLcore;

typedef struct XLcore_list {
    XLcore * core;
    struct XLcore_list * next;
}XLcore_list;

//----------APP----------//


typedef struct XLevent_par{
    event_id_t id;
    mon_id_t mon_id;
    XLsign * sign;
}XLevent_par;

typedef  INS *(*EVENT)(XLevent_par * par);

typedef struct XLevent{
    event_id_t id;
    mon_id_t mon_id;
    EVENT event;
    XLsign sign;
}XLevent;

typedef struct XLapp{
    char name[APP_NAME_LENGTH];
    app_id_t id;
    EVENT event;
}XLapp;

typedef struct XLapp_list{
    XLapp * app;
    struct XLapp_list * next;
}XLapp_list;



typedef struct XLevent_list{
    //event_id_t id;
    //EVENT event;
    //XLqueue_head head;

    XLevent event;
    struct XLevent_list * next;
}XLevent_list;

typedef struct XLevent_thread_arg{
    XLqueue_head head;
    EVENT event;
}XLevent_thread_arg;

//----------MONITOR----------//


typedef struct XLmonitor{
    mon_id_t id;
    //XLsource sender;
    XLsource_list * list;
    XLsource receiver;
    XLqueue_head queue_head;
}XLmonitor;

typedef struct XLmonitor_list{
    XLmonitor * monitor;
    struct XLmonitor_list * next;
}XLmonitor_list;




#endif // TYPE_H
