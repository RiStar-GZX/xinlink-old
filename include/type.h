#ifndef TYPE_H
#define TYPE_H

#define CORE_NAME_LENGTH 64
#define EVENT_NAME_LENGTH 64
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

#ifndef event_id_t
    typedef unsigned int      event_id_t;
#endif

typedef  INS *(*EVENT)(INS * ins);

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
    dev_id_t send_dev;
    //my device
    dev_id_t recv_dev;
    //instruction
    INS * ins;
}XLins;

typedef  struct XLins_decoded_data{
    //str * data_name[64];
    uint8_t *data;
    int datasize;
    struct XLins_decoded_data * next;
}XLins_decoded_data;

typedef  struct XLins_decoded{
    str * event_name[64];
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
//----------EVENT----------//

typedef struct XLevent{
    str name[EVENT_NAME_LENGTH];
    event_id_t id;
    EVENT event;
}XLevent;

typedef struct XLevent_list{
    XLevent * event;
    struct XLevent_list * next;
}XLevent_list;

#endif // TYPE_H
