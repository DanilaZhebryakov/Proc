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
{0x05, instrDup, "Dup"},
{0x06, instrCount, "Count"},
{0x07, instrAdd, "Add"},
{0x08, instrSub, "Sub"},
{0x09, instrMul, "Mul"},
{0x0A, instrBsr, "Bsr"},
{0x0B, instrBsl, "Bsl"},
{0x0C, instrAnd, "And"},
{0x0D, instrDump_0D, "Dump"},
{0x0E, instrOr, "Or"},
{0x0F, instrXor, "Xor"},
{0x10, instrDiv, "Div"},
{0x11, instrSqrt, "Sqrt"},
{0x12, instrPow, "Pow"},
{0x13, instrInp, "Inp"},
{0x14, instrInpCh, "InpCh"},
{0x15, instrOut, "Out"},
{0x16, instrOutCh, "OutCh"},
{0x17, instrJmp, "Jmp", ARG_READ},
{0x18, instrCall, "Call", ARG_READ},
{0x19, instrRet, "Ret"},
{0x1A, instrJgt, "Jgt", ARG_READ},
{0x1B, instrJge, "Jge", ARG_READ},
{0x1C, instrJlt, "Jlt", ARG_READ},
{0x1D, instrJle, "Jle", ARG_READ},
{0x1E, instrJeq, "Jeq", ARG_READ},
{0x1F, instrJne, "Jne", ARG_READ},
};
const int PROC_INSTR_COUNT = sizeof(PROC_INSTR_LIST) / sizeof(Instruction);
