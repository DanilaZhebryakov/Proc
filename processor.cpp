#include <stdio.h>

#include "instructions_lib.h"
#include "logging.h"
#include "Stack.h"

procError_t procRunCode(Processor* proc){
    while (proc->ip - proc->prog_data < proc->prog_size) {
        const uint8_t instr_code = (*(proc->ip)) & MASK_CMD_CODE;
        proc->ip++;

        info_log("Instr: %X ", instr_code);
        procError_t err = PROC_BADCMD;
        for (int i = 0; i < INSTR_COUNT; i++){
            if (instr_code == INSTR_LIST[i].code){
                printf_log("(%s)\n", INSTR_LIST[i].name);
                err = INSTR_LIST[i].func(proc);
                break;
            }
        }

        if (err != PROC_NOERROR){
            return err;
        }
    }
    return PROC_END_OF_CODE;
}


