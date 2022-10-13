#ifndef PROC_UTILS_H_INCLUDED
#define PROC_UTILS_H_INCLUDED

#include "Stack.h"
#include "instructions_utils.h"

enum procError_t{
    PROC_NOERROR     = 0,

    PROC_ERRUNK      = 1 << 0,
    PROC_HALT        = 1 << 1,
    PROC_END_OF_CODE = 1 << 2,

    PROC_BADCMD      = 1 << 3,
    PROC_BADARG      = 1 << 4,
    PROC_STACKEMPT   = 1 << 5,
    PROC_INT_ERROR   = 1 << 6,
    PROC_DIV0        = 1 << 7,
    PROC_BADREG      = 1 << 8
};

struct Processor{
    size_t prog_size;
    const uint8_t* prog_data;
    const uint8_t* ip;
    Stack* stk;
    PROC_DATA_T regs[REG_COUNT];
    PROC_DATA_T* ram;
    size_t ram_size;
};

procError_t getInstrArg(uint8_t instr, Processor* proc, PROC_DATA_T* val);
procError_t setInstrArg(uint8_t instr, Processor* proc, PROC_DATA_T val);

void programDump(const Processor* proc);
void printProcError(procError_t err);



#endif
