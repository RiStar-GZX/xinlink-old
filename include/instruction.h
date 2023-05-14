#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <type.h>
#include <device.h>
#include <string.h>

INS * ins_encode(XLins_decoded * uncode_ins);
XLins_decoded * ins_decode(INS * ins);

#endif // CINSTRUCTION_H