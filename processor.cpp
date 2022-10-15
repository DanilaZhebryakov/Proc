#include <stdio.h>

#include "instr\instructions_proc.h"
#include "lib\logging.h"
#include "lib\Stack.h"
//#define FULL_LOG

procError_t procRun(Processor* proc){
    while (proc->ip - proc->prog_data < proc->prog_size) {
        sf::Event event;
        while ((proc->window)->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                return PROC_USER_STOP;
        }

        const uint8_t instr_code = *(proc->ip);
        proc->ip++;

        #ifdef FULL_LOG
        info_log("Instr: %X ", instr_code);
        #endif
        procError_t err = PROC_BADCMD;
        if (PROC_INSTR_LIST[instr_code].func != nullptr){
            #ifdef FULL_LOG
            printf_log("(%s)\n", PROC_INSTR_LIST[instr_code].name);
            #endif
            proc->ip++;
            err = PROC_INSTR_LIST[instr_code].func(proc);
        }
        if (err != PROC_NOERROR){
            return err;
        }
    }
    return PROC_END_OF_CODE;
}

uint8_t* checkProcSignature(uint8_t* data){
    if (*(uint32_t*)data != SIGNATURE){
        error_log("Error : input program signature bad. Expected %X got %X\n", SIGNATURE, *(uint32_t*)data);
        return nullptr;
    }
    data += sizeof(SIGNATURE);

    if (*(uint16_t*)data != VERSION){
        error_log("Error : input program version bad Expected %d got %d\n", VERSION, *(uint16_t*)data);
        return nullptr;
    }
    data += sizeof(VERSION);
    return data;
}

bool procSetProgram(Processor* proc, uint8_t* data, size_t data_size){
    uint8_t* program_data = checkProcSignature(data);
    if(program_data == nullptr)
        return false;

    proc->prog_data = program_data;
    proc->ip        = program_data;
    proc->prog_size = data_size - (program_data - data);
    return true;
}



