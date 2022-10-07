#ifndef PROC_OBJ_H_INCLUDED
#define PROC_OBJ_H_INCLUDED

#include "file_read.h"

const uint32_t SIGNATURE = 0xDEC0ADDE;
const uint16_t VERSION = 1;

struct Processor{
    size_t prog_size;
    const uint8_t* data;
    const uint8_t* ip;
};

static int getInstrArg(Processor* prog){
    int ret = *(int*)(prog->ip);
    prog->ip += sizeof(int);
    return ret;
}

static void programDump(const Processor* prog){

}
#endif // PROGRAM_H_INCLUDED
