#ifndef TYPE_H
#define TYPE_H

//config
#define PLATFORM_LINUX
//#define PLATFORM_ESP32

#define NETWORK_PORT           8081
#define NETWORK_BROADCAST_PORT 8088

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

#ifndef core_id_t
typedef int core_id_t;
#endif

#ifndef dev_id_t
typedef int      dev_id_t;
#endif


#ifndef INS
typedef char INS;
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

//链表成员结构体
typedef struct XLll_member
{
    uint size;
    void * data;
    struct XLll_member * front;
    struct XLll_member * next;
}XLll_member;

//链表结构体
typedef struct XLll
{
    uint member_num;
    uint member_size;
    struct XLll_member * head;
    struct XLll_member * tail;
}XLll;

//解码后的指令(一个一个字符串)的结构体
typedef  struct XLins_decoded{
    uint8_t ** argv;
    int argc;
}XLins_decoded;

//网络信息结构体
typedef struct XLnet {
    IP ip;
    MAC mac[6];
    PORT port;
}XLnet;

//来源结构体
typedef struct XLsource{
    uint32_t mode;
    uint32_t id;
    XLnet net;
    char * name;
}XLsource;

#define SIZE_SOURCE_WITHOUT_NAME sizeof(uint32_t)*2+sizeof(XLnet)

//----------网络包部分----------//

//通用网络包头部
typedef struct XLpak_base{
    uint16_t mode;
    uint16_t pak_size;
    XLnet net_sender;
    XLnet net_receiver;
}XLpak_base;

//指令网络包结构体
typedef struct XLpak_ins{
    XLpak_base base;
    XLsource sender;
    XLsource receiver;
    uint operate_code;
    INS * ins;
}XLpak_ins;

#define SIZE_PAKINS_WITHOUT_INS sizeof(XLpak_base)+(SIZE_SOURCE_WITHOUT_NAME*2)+sizeof(uint)

//核心对接网络包结构体
typedef struct XLpak_connect{
    XLpak_base base;
    char * core_name;
}XLpak_connect;

//标识符信息
typedef struct XLpak_signinfo{
    uint8_t mode;
    char * name;
    char * type;
}XLpak_signinfo;

//标识符数据包结构体
typedef struct XLpak_sign{
    XLpak_base base;
    uint8_t sign_num;
    XLll * sign_ll;
}XLpak_sign;


//----------队列----------//

//网络包队列共用体
typedef union XLqueue_in{
    XLpak_ins pak_ins;
    XLpak_connect pak_connect;
    XLpak_sign pak_sign;
}XLqueue_in;

//队列成员
typedef struct XLqueue {
    int mode;
    XLqueue_in in;
    LEVEL level;
    struct XLqueue * front;
    struct XLqueue * next;
}XLqueue;

//队列头信息
typedef struct XLqueue_head {
    XLqueue * queue;
}XLqueue_head;

//设备标识符结构体
typedef struct XLsign{
    uint8_t use; //是否使用设备标识符
    uint8_t mode;
    char * name;
    char * type;
    void * contrast;
    int contrast_size;
}XLsign;

//核心结构体
typedef struct XLcore {
    char name[CORE_NAME_LENGTH];
    core_id_t id;
    XLnet net;
    uint8_t safety;
    XLll * sign_ll;
}XLcore;

//传入事件函数的参数
typedef struct XLevent_par{
    XLsource * source;
    XLsign * sign;
    XLpak_ins * pak_ins;
}XLevent_par;

//事件函数指针
typedef  void *(*EVENT)(XLevent_par * par);

//事件结构体
typedef struct XLevent{
    event_id_t id;
    mon_id_t mon_id;
    EVENT event;
    EVENT init;
    EVENT exit;
    XLsign sign;
}XLevent;

//应用结构体
typedef struct XLapp{
    char name[APP_NAME_LENGTH];
    app_id_t id;
    EVENT event;
    EVENT init;
    EVENT exit;
}XLapp;

//监视器结构体
typedef struct XLmonitor{
    mon_id_t id;
    //XLsource sender;
    XLll source_ll;
    XLsource receiver;
    XLqueue_head queue_head;
}XLmonitor;

#endif // TYPE_H
