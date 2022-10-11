#ifndef PROC_UTILS_H_INCLUDED
#define PROC_UTILS_H_INCLUDED

#include "Stack.h"

const uint32_t SIGNATURE = 0xDEC0ADDE;
const uint16_t VERSION = 15;

const uint8_t MASK_CMD_CODE = 0x1F;
const uint8_t MASK_CMD_IMM  = 0x20;
const uint8_t MASK_CMD_REG  = 0x40;
const uint8_t MASK_CMD_MEM  = 0x80;

const uint8_t REG_COUNT = 15;

enum instrArgReq_t{
    ARG_NOARG = 0,
    ARG_READ = 1,
    ARG_WRITE = 2
};

bool matchesArgReq(uint8_t instr, instrArgReq_t req);

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
    int regs[REG_COUNT];
    int* ram;
    size_t ram_size;
};

procError_t getInstrArg(uint8_t instr, Processor* proc, int* val);
procError_t setInstrArg(uint8_t instr, Processor* proc, int val);

void programDump(const Processor* proc);
void printProcError(procError_t err);



#endif
