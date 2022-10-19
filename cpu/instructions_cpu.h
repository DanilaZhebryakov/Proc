#ifndef INSTRUCTIONS_H_INCLUDED
#define INSTRUCTIONS_H_INCLUDED
#include<stdint.h>

#define ELEM_T int
#include "proc_utils.h"

struct Instruction{
    uint8_t code;
    procError_t (*func)(Processor*);
    const char* name;
};

extern const struct Instruction PROC_INSTR_LIST[];
extern const int PROC_INSTR_COUNT;

#endif // INSTRUCTIONS_H_INCLUDED
