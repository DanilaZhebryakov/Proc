#include <stdio.h>
#include "instructions_lib.h"
#include "instructions.h"
// WARNING: auto-generated!
//DO NOT CHANGE

const struct Instruction INSTR_LIST[] = {
{0x00, instrBad, "Bad"},
{0x01, instrNop, "Nop"},
{0x02, instrHalt, "Halt"},
{0x03, instrDump, "Dump"},
{0x04, instrPop, "Pop", ARG_WRITE},
{0x05, instrPush, "Push", ARG_READ},
{0x06, instrSwap, "Swap"},
{0x07, instrDup, "Dup"},
{0x08, instrCount, "Count"},
{0x09, instrGet, "Get"},
{0x0A, instrAdd, "Add"},
{0x0B, instrSub, "Sub"},
{0x0C, instrMul, "Mul"},
{0x0D, instrDiv, "Div"},
{0x0E, instrPow, "Pow"},
{0x0F, instrInp, "Inp"},
{0x10, instrInpCh, "InpCh"},
{0x11, instrOut, "Out"},
{0x12, instrOutCh, "OutCh"},
{0x13, instrJmp, "Jmp", ARG_READ},
{0x14, instrJgt, "Jgt", ARG_READ},
{0x15, instrJge, "Jge", ARG_READ},
{0x16, instrJlt, "Jlt", ARG_READ},
{0x17, instrJle, "Jle", ARG_READ},
{0x18, instrJeq, "Jeq", ARG_READ},
{0x19, instrJne, "Jne", ARG_READ},
};
const int INSTR_COUNT = sizeof(INSTR_LIST) / sizeof(Instruction);
