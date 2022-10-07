#ifndef PROC_OBJ_H_INCLUDED
#define PROC_OBJ_H_INCLUDED


#include "file_read.h"
#include "Stack.h"

const uint32_t SIGNATURE = 0xDEC0ADDE;
const uint16_t VERSION = 1;

struct Processor{
    size_t prog_size;
    const uint8_t* prog_data;
    const uint8_t* ip;
    Stack* stk;
};

static int getInstrArg(Processor* prog){
    int ret = *(int*)(prog->ip);
    prog->ip += sizeof(int);
    return ret;
}

static void programDump(const Processor* proc){
    info_log("Program dump\n");
    for (const uint8_t* i = proc->prog_data; i - proc->prog_data < proc->prog_size; i++){
        printf_log("%02X ", *i);
    }
    printf_log("\n");
    for (const uint8_t* i = proc->prog_data; i < proc->ip; i++){
        printf_log("---", *i);
    }
    printf_log("^^\n");
    printf_log("(%d/%d)\n", proc->ip - proc->prog_data, proc->prog_size);
    stackDump(proc->stk);
}
#endif // PROGRAM_H_INCLUDED
