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
{0x05, "Swap"},
{0x06, "Dup"},
{0x07, "Count"},
{0x08, "Get"},
{0x09, "Add"},
{0x0A, "Sub"},
{0x0B, "Mul"},
{0x0C, "Div"},
{0x0D, "Dump"},
{0x0E, "Pow"},
{0x0F, "Inp"},
{0x10, "InpCh"},
{0x11, "Out"},
{0x12, "OutCh"},
{0x13, "Jmp", ARG_READ},
{0x14, "Jgt", ARG_READ},
{0x15, "Jge", ARG_READ},
{0x16, "Jlt", ARG_READ},
{0x17, "Jle", ARG_READ},
{0x18, "Jeq", ARG_READ},
{0x19, "Jne", ARG_READ},
};
const int CMP_INSTR_COUNT = sizeof(CMP_INSTR_LIST) / sizeof(CompileInstruction);
