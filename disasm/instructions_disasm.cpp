#include <stdio.h>
#include "instructions_disasm.h"
// WARNING: auto-generated!
//DO NOT CHANGE

const struct CompileInstruction CMP_INSTR_LIST[] = {
 {0x00, "Bad", },
 {0x01, "Nop", },
 {0x02, "Halt", },
 {0x03, "Pause", },
 {0x04, "Pop", ARG_WRITE},
 {0x05, "Push", ARG_READ},
 {0x06, "Swap", },
 {0x07, "Dup", },
 {0x08, "Count", },
 {0x09, "Get", },
 {0x0A, "Add", },
 {0x0B, "Sub", },
 {0x0C, "Mul", },
 {0x0D, "Dump", },
 {0x0E, "Bsr", },
 {0x0F, "Bsl", },
 {0x10, "And", },
 {0x11, "Or", },
 {0x12, "Xor", },
 {0x13, "Div", },
 {0x14, "Sqrt", },
 {0x15, "Pow", },
 {0x16, "Inp", },
 {0x17, "InpCh", },
 {0x18, "Out", },
 {0x19, "OutCh", },
 {0x1A, "Jmp", ARG_READ},
 {0x1B, "Call", ARG_READ},
 {0x1C, "Ret", },
 {0x1D, "Jgt", ARG_READ},
 {0x1E, "Jge", ARG_READ},
 {0x1F, "Jlt", ARG_READ},
 {0x20, "Jle", ARG_READ},
 {0x21, "Jeq", ARG_READ},
 {0x22, "Jne", ARG_READ},
};
const int CMP_INSTR_COUNT = sizeof(CMP_INSTR_LIST) / sizeof(CompileInstruction);
