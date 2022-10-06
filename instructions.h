#ifndef INSTRUCTIONS_H_INCLUDED
#define INSTRUCTIONS_H_INCLUDED
#include<stdint.h>

#define ELEM_T int
#include "Stack.h"
#include "program.h"

enum procError_t{
    PROC_NOERROR = 0,
    PROC_ERRUNK  = 1,
    PROC_HALT    = 2,

    PROC_BADCMD      = 1 << 3,
    PROC_STACKEMPT   = 1 << 4,
    PROC_INT_ERROR   = 1 << 5,
    PROC_DIV0        = 1 << 6
};

struct Instruction{
    uint8_t code;
    procError_t (*func)(Stack* ,Program* prog);
    const char* name;
};

extern const struct Instruction INSTR_LIST[];
extern const int INSTR_COUNT;

#endif // INSTRUCTIONS_H_INCLUDED
