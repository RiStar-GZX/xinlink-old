#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <type.h>
#include <string.h>

#define MONITOR_MODE_NONE       5

INS * ins_encode(XLins_decoded * uncode_ins);
XLins_decoded * ins_decode(INS * ins);
int ins_send_to_event(XLpak_ins * ins);

mon_id_t monitor_create(XLsource * sender,XLsource * receiver);
void monitor_list(void);
int monitor_remove(mon_id_t id);
XLpak_ins * monitor_get_member(mon_id_t monitor_id);
int monitor_remove_member(mon_id_t monitor_id);
#endif // CINSTRUCTION_H
