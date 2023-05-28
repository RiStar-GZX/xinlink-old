#ifndef TYPE_H
#define TYPE_H

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


#ifndef int8_t
    typedef               char     int8;
#endif

#ifndef int16_t
    typedef    short      int      int16;
#endif

#ifndef int32_t
    typedef               int      int32;
#endif

#ifndef str
    typedef char str;
#endif

#ifndef core_id_t
    typedef int core_id_t;
#endif

#ifndef dev_id_t
    typedef int      dev_id_t;
#endif


#ifndef INS
    typedef str INS;
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


//----------DEV----------//
typedef struct XLdev{
    dev_id_t id;
    str name[CORE_NAME_LENGTH];
}XLdev;

typedef struct XLdev_list{
    XLdev * dev;
    struct XLdev_list * next;
}XLdev_list;
//----------INS----------//




typedef struct XLins{
    int mode;
    //sender core
    int core_id;
    //sender device
    event_id_t send_event_id;
    //my device
    event_id_t recv_event_id;
    str recv_app_name[APP_NAME_LENGTH];
    //instruction
    INS * INS;
}XLins;

typedef  struct XLins_decoded_data{
    //str * data_name[64];
    uint8_t *data;
    int datasize;
    struct XLins_decoded_data * next;
}XLins_decoded_data;

typedef  struct XLins_decoded{
    str app_name[64];
    XLins_decoded_data * data_list;
}XLins_decoded;
//----------NET----------//

typedef struct XLnet {
    IP ip;
    MAC mac[6];
    PORT port;
}XLnet;

typedef struct XLins_queue {
    XLins * ins;
    LEVEL level;
    struct XLins_queue * front;
    struct XLins_queue * next;
}XLins_queue;

//----------CORE----------//

typedef struct XLcore {
    str name[CORE_NAME_LENGTH];
    core_id_t id;
    XLnet net;
}XLcore;

typedef struct XLcore_list {
    XLcore * core;
    struct XLcore_list * next;
}XLcore_list;
//----------APP----------//
typedef  INS *(*EVENT)(XLins * ins);

typedef struct XLapp{
    str name[APP_NAME_LENGTH];
    app_id_t id;
    EVENT event;
}XLapp;

typedef struct XLapp_list{
    XLapp * app;
    struct XLapp_list * next;
}XLapp_list;


typedef struct XLevent_list{
    event_id_t id;
    EVENT event;
    struct XLevent_list * next;
}XLevent_list;

typedef struct XLevent_thread_arg{
    XLins ins;
    EVENT event;
}XLevent_thread_arg;


#endif // TYPE_H
