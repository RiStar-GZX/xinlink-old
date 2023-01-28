#ifndef LINK_H
#define LINK_H

#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>
#include<string.h>
#include <type.h>

#define RELATE_HARD_NULL 0
#define RELATE_HARD_1p 1
#define RELATE_HARD_2p 2

#define RELATE_SOFT_NULL 0
#define RELATE_SOFT_1p 1
#define RELATE_SOFT_2p 2

#define NETWORK_IPV4 1
#define NETWORK_IPV6 2

#define NETWORK_IPV4 1
#define NETWORK_IPV6 2

#define DEV_TYPE_LO 0
#define DEV_TYPE_NET 1

/*enum INS_TYPE{
    INS_DEV_CREATE =1,
    INS_DEV_DEL ,
    INS_DEV_INIT,
    INS_DEV_SET_NAME,
    INS_DEV_SET_NET ,
    INS_DEV_SET_SIG ,
    INS_DEV_GET_NAME,
    INS_DEV_GET_NET,
    INS_DEV_GET_SIG,

    INS_NETDEV_CREATE,
    INS_NETDEV_DEL ,
    INS_NETDEV_GET_NAME,
    INS_NETDEV_GET_NET,
    INS_NETDEV_GET_SIG,

    INS_SIG_CREATE,
    INS_SIG_DEL ,
    INS_SIG_SET_NAME,
    INS_SIG_SET_DEV ,
    INS_SIG_GET_NAME,
    INS_SIG_GET_DEV,
    INS_CONNECT
}INS_TYPE;

enum RES_TYPE{
    RES_VOID =1,
    RES_INT8_T,
    RES_UINT16_T,
    RES_STR
}RES_TYPE;*/
typedef struct ins_cts_shm
{
    uint16_t ins_type;
    uint16_t dev1_id;
    uint8_t dev1_type;
    str dev1_name[DEVICE_NAME_LENGTH];
    uint16_t dev2_id;
    uint8_t dev2_type;
    str dev2_name[DEVICE_NAME_LENGTH];
    uint16_t sig_id;
    str sig_name[sig_NAME_LENGTH];
}ins_cts_shm;

union result{
    int8_t result;
    uint16_t id;
    str str[DEVICE_NAME_LENGTH];
};

typedef struct ins_stc_shm
{
    uint16_t ins_type;
    uint16_t res_type;
    union result result;
}ins_stc_shm;

typedef struct ins_shm
{
    uint8_t state;
    uint32_t ins_id;
    struct ins_cts_shm cts;
    struct ins_stc_shm stc;
}ins_shm;

struct ins_shm *shm_create(int id);
struct ins_shm *shm_get(int id);
#endif
