#include <stdio.h>
#include <unistd.h>

#include "lib\logging.h"
#include "proc_utils.h"

void procCtor(Processor* proc){
    Stack* stk = (Stack*)calloc(1,sizeof(*stk));
    stackCtor(stk);
    proc->stk = stk;
    proc->ram_size = PROC_DEFAULT_RAM_SIZE;
    proc->ram = (PROC_DATA_T*)calloc(proc->ram_size, sizeof(PROC_DATA_T));
}

void procDtor(Processor* proc){
    stackDtor(proc->stk);

    free(proc->stk);
    proc->stk = nullptr;
    free(proc->ram);
    proc->ram = nullptr;
    proc->ram_size = 0;
}

procError_t getInstrArg(uint8_t instr, Processor* proc, PROC_DATA_T* val){
    if (!hasValidReadArg(instr)) {
        return PROC_BADARG;
    }
    PROC_DATA_T r = 0;
    if (instr & MASK_CMD_REG) {
        uint8_t regn = *proc->ip;
        if (regn == 0 || regn > REG_COUNT){
            return PROC_BADREG;
        }
        r += proc->regs[regn - 1];
        proc->ip += 1;
    }
    if (instr & MASK_CMD_IMM) {
        r += *(PROC_DATA_T*)(proc->ip);
        proc->ip += sizeof(PROC_DATA_T);
    }
    if (instr & MASK_CMD_MEM) {
        sf::sleep(sf::milliseconds(100));
        if(r < 0 || r >= proc->ram_size){
            return PROC_BADRAM;
        }

        r = proc->ram[r];
    }

    *val = r;
    return PROC_NOERROR;
}

procError_t setInstrArg(uint8_t instr, Processor* proc, PROC_DATA_T val){
    if (!hasValidWriteArg(instr)){
        return PROC_BADARG;
    }

    uint8_t regn = 0;
    PROC_DATA_T r = 0;
    if (instr & MASK_CMD_REG) {
        regn = *proc->ip;
        if (regn == 0 || regn > REG_COUNT){
            return PROC_BADREG;
        }
        r = proc->regs[regn - 1];
        proc->ip += 1;
    }

    if (instr & MASK_CMD_IMM){
        r += *(PROC_DATA_T*)(proc->ip);
        proc->ip += sizeof(PROC_DATA_T);
    }

    if (instr & MASK_CMD_MEM){
        sf::sleep(sf::milliseconds(100));
        if(r < 0 || r >= proc->ram_size){
            return PROC_BADRAM;
        }
        (*(proc->window)).display();
        proc->ram[r] = val;
        sf::RectangleShape shape(sf::Vector2f(PROC_PX_SIZE, PROC_PX_SIZE));
        shape.setPosition(sf::Vector2f(PROC_PX_SIZE * (r % PROC_WINDOW_WIDTH),PROC_PX_SIZE * (r / PROC_WINDOW_WIDTH)));
        shape.setFillColor(sf::Color((val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF));
        (*(proc->window)).draw(shape);
        (*(proc->window)).display();
    }
    else{
        proc->regs[regn - 1] = val;
    }
    return PROC_NOERROR;
}

void programDump(const Processor* proc){
    info_log("Program dump\n");
    for (const uint8_t* i = proc->prog_data; i < proc->ip; i++){
        printf_log("%02X-", *i);
    }
    for (const uint8_t* i = proc->ip; (i - proc->prog_data) < proc->prog_size; i++){
        printf_log("%02X ", *i);
    }

    printf_log("(%d/%d)\n", proc->ip - proc->prog_data, proc->prog_size);

    printf_log("    Registers: ");
    for (int i = 0; i < REG_COUNT; i++){
        printf_log(PROC_DATA_SPEC " ", proc->regs[i]);
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
    if (err & PROC_BADRAM)
        printf_log(" Address out of memory");
    if (err & PROC_ERRUNK)
        printf_log(" Unknown error");
    if (err & PROC_END_OF_CODE)
        printf_log(" Reached end of program");
    if (err & PROC_USER_STOP)
        printf_log(" Stopped by user");
    if (err & PROC_BADCMD)
        printf_log(" Instruction not found");
    if (err & PROC_STACKEMPT)
        printf_log(" Not enough elements in stack");
    if (err & PROC_INT_ERROR)
        printf_log(" Internal error");
    if (err & PROC_BADMATH)
        printf_log(" Impossible math operation");
    printf_log("\n");
}
