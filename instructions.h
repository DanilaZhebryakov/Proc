#ifndef INSTRUCTIONS_H_INCLUDED
#define INSTRUCTIONS_H_INCLUDED
#include<stdint.h>

#define ELEM_T int
#include "proc_utils.h"

struct Instruction{
    uint8_t code;
    procError_t (*func)(Processor*);
    const char* name;
    instrArgReq_t arg_req;
};

extern const struct Instruction INSTR_LIST[];
extern const int INSTR_COUNT;

#endif // INSTRUCTIONS_H_INCLUDED
