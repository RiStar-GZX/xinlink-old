#ifndef DEVICE_H
#define DEVICE_H

#include<stdio.h>
#include<malloc.h>
#include<string.h>

#include <type.h>

#define CORE_NAME_LENGTH 64

#ifndef dev_id_t
    typedef int      dev_id_t;
#endif

typedef struct XLdev{
    dev_id_t id;
    str name[CORE_NAME_LENGTH];
}XLdev;

typedef struct XLdev_list{
    XLdev * dev;
    struct XLdev_list * next;
}XLdev_list;

#endif // DEVICE_H
