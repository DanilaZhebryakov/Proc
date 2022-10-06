#ifndef PROGRAM_H_INCLUDED
#define PROGRAM_H_INCLUDED

#include "file_read.h"

struct Program{
    size_t size;
    const Text* data;
    int ip;
    const char* arg_ptr;
};

static int getInstrArg(Program* prog){
    int ret = 0;
    sscanf(prog->arg_ptr, " %d", &ret);
    return ret;
}

static void programDump(const Program* prog){

}



#endif // PROGRAM_H_INCLUDED
