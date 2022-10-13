#include <stdio.h>

#include "instructions_proc.h"
#include "logging.h"
#include "Stack.h"

procError_t procRunCode(Processor* proc){
    while (proc->ip - proc->prog_data < proc->prog_size) {
        const uint8_t instr_code = (*(proc->ip)) & MASK_CMD_CODE;
        proc->ip++;

        info_log("Instr: %X ", instr_code);
        procError_t err = PROC_BADCMD;
        if (PROC_INSTR_LIST[instr_code].func != nullptr){
            printf_log("(%s)\n", PROC_INSTR_LIST[instr_code].name);
            err = PROC_INSTR_LIST[instr_code].func(proc);
        }
        if (err != PROC_NOERROR){
            return err;
        }
    }
    return PROC_END_OF_CODE;
}


