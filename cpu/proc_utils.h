#ifndef PROC_UTILS_H_INCLUDED
#define PROC_UTILS_H_INCLUDED

#include "lib\Stack.h"
#include "instr\instructions_utils.h"
#include  <SFML/Graphics.hpp>

const int PROC_DEFAULT_RAM_SIZE = 400;
const int PROC_WINDOW_WIDTH  = 20;
const int PROC_WINDOW_HEIGHT = 20;
const int PROC_PX_SIZE       = 10;

enum procError_t{
    PROC_NOERROR     = 0,

    PROC_ERRUNK      = 1 << 0,
    PROC_HALT        = 1 << 1,
    PROC_END_OF_CODE = 1 << 2,
    PROC_USER_STOP   = 1 << 3,

    PROC_BADCMD      = 1 << 4,
    PROC_BADARG      = 1 << 5,
    PROC_STACKEMPT   = 1 << 6,
    PROC_INT_ERROR   = 1 << 7,
    PROC_BADMATH     = 1 << 8,
    PROC_BADRAM      = 1 << 9,
    PROC_BADREG      = 1 << 10,
    PROC_BADPROC     = 1 << 11
};

struct Processor{
    size_t prog_size;
    const uint8_t* prog_data;
    const uint8_t* ip;
    Stack* stk;
    PROC_DATA_T regs[REG_COUNT];
    PROC_DATA_T* ram;
    size_t ram_size;
    sf::RenderWindow* window;
};

void procCtor(Processor* proc);
void procDtor(Processor* proc);

procError_t procError(Processor* proc);
procError_t getInstrArg(uint8_t instr, Processor* proc, PROC_DATA_T* val);
procError_t setInstrArg(uint8_t instr, Processor* proc, PROC_DATA_T val);

void programDump(const Processor* proc);
void printProcError(procError_t err);



#endif
