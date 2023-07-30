#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <type.h>
#include <string.h>

//#define MONITOR_MODE_NONE       5
#define MONITOR_MODE_ALL      6

INS * ins_encode(XLins_decoded * uncode_ins);
XLins_decoded * ins_decode(INS * ins);
int ins_send_to_event(XLpak_ins * ins);

mon_id_t monitor_create(XLsource_list * send_list,XLsource * receive);
void monitor_list(void);
int monitor_remove(mon_id_t id);
XLpak_ins * monitor_get_member(mon_id_t monitor_id);
INS * monitor_get_ins(mon_id_t monitor_id);
int monitor_remove_member(mon_id_t monitor_id);
int monitor_limit_add_source(mon_id_t mon_id,XLsource * source);

int monitor_set(mon_id_t id,XLsource * sender,XLsource * receiver);
XLsource monitor_get_source(mon_id_t id);

int ins_get_par(INS * ins,char *name);
char * ins_get_par_str(INS * ins,char *name);
int ins_get_par_int(INS *ins,char* name,int defalut);
int ins_get_par_ip(INS * ins,char *name,IP * ip);
int ins_get_par_source(INS *ins,char *name,XLsource * source);
char * ins_source_to_str(XLsource *source);

int source_cmp(XLsource *source1,XLsource *source2);
XLsource_list * source_list_create(void);

#endif // CINSTRUCTION_H
