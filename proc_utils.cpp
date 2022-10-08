#include <stdio.h>

#include "logging.h"
#include "proc_utils.h"


int getInstrArg(Processor* prog){
    int ret = *(int*)(prog->ip);
    prog->ip += sizeof(int);
    return ret;
}

void programDump(const Processor* proc){
    info_log("Program dump\n");
    for (const uint8_t* i = proc->prog_data; (i - proc->prog_data) < proc->prog_size; i++){
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

void printProcError(procError_t err){
    info_log("Program stopped:");
    if (err & PROC_HALT)
        printf_log(" Halted");
    if (err & PROC_ERRUNK)
        printf_log(" Unknown error");
    if (err & PROC_END_OF_CODE)
        printf_log(" Reached end of program");
    if (err & PROC_BADCMD)
        printf_log(" Instruction not found");
    if (err & PROC_STACKEMPT)
        printf_log(" Not enough elements in stack");
    if (err & PROC_INT_ERROR)
        printf_log(" Internal error");
    if (err & PROC_DIV0)
        printf_log(" Zero division error");
    printf_log("\n");
}
