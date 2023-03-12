#ifndef TYPE_H
#define TYPE_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/* base */

typedef unsigned              char     uint8_t;

typedef unsigned    short     int      uint16_t;

typedef unsigned              int      uint32_t;

//typedef unsigned            long long  uint64_t;

/*
typedef               char     int8_t;

typedef    short      int      int16_t;

typedef               int      int32_t;

typedef    long       long     int64_t;
*/

typedef char str;

/*  */


/*  */

typedef uint16_t  rel_mod_t;

typedef uint16_t  i_t;
typedef uint16_t  dev_id_t;
typedef uint16_t  sig_id_t;
typedef uint16_t core_id_t;
typedef uint16_t  netdev_id_t;

//typedef uint16_t  dev_type_t;
typedef uint8_t  par_data_t;

typedef uint32_t  par_type_t;
typedef uint8_t tree_type_t;
typedef int8_t  mod_t;
typedef uint8_t  init_type_t;
typedef uint8_t  dev_type_t;
#define DEVICE_TYPE_SN 1

//typedef int (*func_name)(void) SLOT




/*typedef struct {
	int (*func_name)(void);
} SIG_SHOT;
*/
/*  */


#define USE_IPV4
#define USE_IPV6
#define DEVICE_NAME_LENGTH 64
#define PAR_NAME_LENGTH 32
#define DEVICE_TYPE_NAME_LENGTH 16
#define DEVICE_IO_NAME_LENGTH 16

#define sig_NAME_LENGTH 64

#define BELONGER_


#endif // TYPE_H
