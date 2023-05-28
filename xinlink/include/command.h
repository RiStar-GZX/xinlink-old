#ifndef COMMAND_H
#define COMMAND_H

#include <type.h>

#ifndef XLcmd
    typedef str XLcmd;
#endif

typedef struct XLcmd_list{
    XLcmd * cmd;
    struct XLcmd_list * next;
}XLcmd_list;

#endif // COMMAND_H
