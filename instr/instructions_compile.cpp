#include <stdio.h>
#include "instructions_compile.h"
// WARNING: auto-generated!
//DO NOT CHANGE

const struct CompileInstruction CMP_INSTR_LIST[] = {
{0x00, "Bad"},
{0x01, "Nop"},
{0x02, "Halt"},
{0x03, "Pop", ARG_WRITE},
{0x04, "Push", ARG_READ},
{0x05, "Dup"},
{0x06, "Count"},
{0x07, "Add"},
{0x08, "Sub"},
{0x09, "Mul"},
{0x0A, "Bsr"},
{0x0B, "Bsl"},
{0x0C, "And"},
{0x0D, "Dump"},
{0x0E, "Or"},
{0x0F, "Xor"},
{0x10, "Div"},
{0x11, "Sqrt"},
{0x12, "Pow"},
{0x13, "Inp"},
{0x14, "InpCh"},
{0x15, "Out"},
{0x16, "OutCh"},
{0x17, "Jmp", ARG_READ},
{0x18, "Call", ARG_READ},
{0x19, "Ret"},
{0x1A, "Jgt", ARG_READ},
{0x1B, "Jge", ARG_READ},
{0x1C, "Jlt", ARG_READ},
{0x1D, "Jle", ARG_READ},
{0x1E, "Jeq", ARG_READ},
};
const int CMP_INSTR_COUNT = sizeof(CMP_INSTR_LIST) / sizeof(CompileInstruction);
