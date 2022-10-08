#include <stdio.h>
#include <unistd.h>

#include "logging.h"
#include "proc_utils.h"

bool hasValidReadArg(uint8_t instr){
    return instr & (MASK_CMD_REG | MASK_CMD_IMM);
}
bool hasValidWriteArg(uint8_t instr){
    return (instr & MASK_CMD_REG) || (instr & (MASK_CMD_IMM | MASK_CMD_MEM));
}

bool matchesArgReq(uint8_t instr, instrArgReq_t req){
    switch(req){
    case ARG_NOARG:
        return !hasValidReadArg(instr);
    case ARG_READ:
        return hasValidReadArg(instr);
    case ARG_WRITE:
        return hasValidWriteArg(instr);
    default:
        return false;
    }
}

procError_t getInstrArg(uint8_t instr, Processor* proc, int* val){
    if (!hasValidReadArg(instr)) {
        return PROC_BADARG;
    }
    int r = 0;
    if (instr & MASK_CMD_REG) {
        uint8_t regn = *proc->ip;
        if(regn == 0 || regn > REG_COUNT){
            return PROC_BADREG;
        }
        r += proc->regs[regn - 1];
        proc->ip += 1;
    }
    if (instr & MASK_CMD_IMM) {
        r += *(int*)(proc->ip);
        proc->ip += sizeof(int);
    }
    if (instr & MASK_CMD_MEM) {
        sleep(1);
        r = proc->ram[r];
    }

    *val = r;
    return PROC_NOERROR;
}

procError_t setInstrArg(uint8_t instr, Processor* proc, int val){
    if (!hasValidWriteArg(instr)){
        return PROC_BADARG;
    }

    uint8_t regn = 0;
    int r = 0;
    if (instr & MASK_CMD_REG) {
        regn = *proc->ip;
        if(regn == 0 || regn > REG_COUNT){
            return PROC_BADREG;
        }
        r = proc->regs[regn - 1];
        proc->ip += 1;
    }

    if (instr & MASK_CMD_IMM){
        r += *(int*)(proc->ip);
        proc->ip += sizeof(int);
    }

    if (instr & MASK_CMD_MEM){
        sleep(1);
        proc->ram[r] = val;
    }
    else{
        proc->regs[regn - 1] = val;
    }

    return PROC_NOERROR;
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

    printf_log("    Registers: ");
    for(int i = 0; i < REG_COUNT; i++){
        printf_log("%d ", proc->regs[i]);
    }
    printf_log("\n");

    stackDump(proc->stk);
}

void printProcError(procError_t err){
    info_log("Program stopped:");
    if (err & PROC_HALT)
        printf_log(" Halted");
    if (err & PROC_BADCMD)
        printf_log(" Bad command");
    if (err & PROC_BADARG)
        printf_log(" Bad argument");
    if (err & PROC_BADREG)
        printf_log(" Bad register");
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
