#include <stdio.h>
#include "instructions_proc.h"
#include "instructions.h"
// WARNING: auto-generated!
//DO NOT CHANGE

const struct Instruction PROC_INSTR_LIST[] = {
{0x00, instrBad, "Bad"},
{0x01, instrNop, "Nop"},
{0x02, instrHalt, "Halt"},
{0x03, instrPop, "Pop", ARG_WRITE},
{0x04, instrPush, "Push", ARG_READ},
{0x05, instrSwap, "Swap"},
{0x06, instrDup, "Dup"},
{0x07, instrCount, "Count"},
{0x08, instrGet, "Get"},
{0x09, instrAdd, "Add"},
{0x0A, instrSub, "Sub"},
{0x0B, instrMul, "Mul"},
{0x0C, instrDiv, "Div"},
{0x0D, instrDump_0D, "Dump"},
{0x0E, instrSqrt, "Sqrt"},
{0x0F, instrPow, "Pow"},
{0x10, instrInp, "Inp"},
{0x11, instrInpCh, "InpCh"},
{0x12, instrOut, "Out"},
{0x13, instrOutCh, "OutCh"},
{0x14, instrJmp, "Jmp", ARG_READ},
{0x15, instrCall, "Call", ARG_READ},
{0x16, instrRet, "Ret"},
{0x17, instrJgt, "Jgt", ARG_READ},
{0x18, instrJge, "Jge", ARG_READ},
{0x19, instrJlt, "Jlt", ARG_READ},
{0x1A, instrJle, "Jle", ARG_READ},
{0x1B, instrJeq, "Jeq", ARG_READ},
{0x1C, instrJne, "Jne", ARG_READ},
{0x1D},
{0x1E},
{0x1F},
};
const int PROC_INSTR_COUNT = sizeof(PROC_INSTR_LIST) / sizeof(Instruction);
