#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <type.h>
#include <device.h>
#include <string.h>

#ifndef INS
    typedef str INS;
#endif

typedef struct XLins{
    //sender core
    int core_id;
    //sender device
    XLdev * send_dev;
    //my device
    XLdev * recv_dev;
    //instruction
    INS * ins;
}XLins;


typedef  struct XLins_decoded_data{
    str * data_name[64];
    uint8_t *data;
    int datasize;
    struct XLins_decoded_data * next;
}XLins_decoded_data;

typedef  struct XLins_decoded{
    str * event_name[64];
    XLins_decoded_data * data_list;
}XLins_decoded;

INS * ins_encode(XLins_decoded * uncode_ins);
XLins_decoded * ins_decode(INS * ins);

#endif // CINSTRUCTION_H
