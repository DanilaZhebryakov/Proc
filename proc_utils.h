#ifndef PROC_UTILS_H_INCLUDED
#define PROC_UTILS_H_INCLUDED

#include "Stack.h"

const uint32_t SIGNATURE = 0xDEC0ADDE;
const uint16_t VERSION = 1;

enum procError_t{
    PROC_NOERROR     = 0,

    PROC_ERRUNK      = 1 << 0,
    PROC_HALT        = 1 << 1,
    PROC_END_OF_CODE = 1 << 2,

    PROC_BADCMD      = 1 << 3,
    PROC_STACKEMPT   = 1 << 4,
    PROC_INT_ERROR   = 1 << 5,
    PROC_DIV0        = 1 << 6
};

struct Processor{
    size_t prog_size;
    const uint8_t* prog_data;
    const uint8_t* ip;
    Stack* stk;
};

int getInstrArg(Processor* prog);
void programDump(const Processor* proc);
void printProcError(procError_t err);

#endif
