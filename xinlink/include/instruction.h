#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <type.h>
#include <device.h>
#include <string.h>

#define MODE_EVENT_ID 0
#define MODE_DEV_ID 1
#define MODE_ACCESS_ID 2
#define MODE_APP_NAME 3

INS * ins_encode(XLins_decoded * uncode_ins);
XLins_decoded * ins_decode(INS * ins);
int ins_send_to_event(XLins * ins);

int ins_sender_set_id(XLins * ins,int mode,int id);
int ins_recevier_set_id(XLins * ins,int mode,int id);
int ins_recevier_set_appname(XLins * ins,str *name);

XLins_queue_head * monitor(XLsource * sender,XLsource *recevier);
#endif // CINSTRUCTION_H
