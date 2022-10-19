#include "instructions_disasm.h"
#include <stdio.h>

void printInstrArg(FILE* file, uint8_t instr, uint8_t** arg_ptr){

    if (instr & MASK_CMD_MEM)
        fprintf(file, "[");

    if (instr & MASK_CMD_REG) {
        fprintf(file, "r%d", **arg_ptr);
        *arg_ptr += 1;
        if (instr & MASK_CMD_IMM)
            fprintf(file, " + ");
    }
    if (instr & MASK_CMD_IMM) {
        fprintf(file, "%d", *((PROC_DATA_T*)(*arg_ptr)));
        *arg_ptr += sizeof(PROC_DATA_T);
    }

    if (instr & MASK_CMD_MEM)
        fprintf(file, "]");
}

void printDisassembly(FILE* file, uint8_t* input_data, size_t data_size){
    printf("Siganture: 0x%08X Version: %u\n", *(int*)input_data, *(input_data + sizeof(SIGNATURE)));


    uint8_t* ip = input_data + sizeof(SIGNATURE) + sizeof(VERSION);
    while(ip < input_data + data_size){
        int instr_status = 0;
        for (int i = 0; i < CMP_INSTR_COUNT; i++){
            if (CMP_INSTR_LIST[i].code == *ip){
                instr_status = 1;
                fprintf(file, "%s ", CMP_INSTR_LIST[i].name);
                ip+=2;
                printInstrArg(file, *(ip-1), &ip);
                printf("\n");
                break;
            }
        }
        if(instr_status == 0){
            fprintf(file, "[Bad instruction] Code %02X \n", ((*ip) & MASK_CMD_CODE));
            ip++;
        }
    }
}
