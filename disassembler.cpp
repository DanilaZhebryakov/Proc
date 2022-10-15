#include <stdio.h>

#include "lib\logging.h"
#include "lib\parseArg.h"
#include "lib\file_read.h"

#include "instr\instructions_compile.h"

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

int main(int argc, const char *argv[]){
    initConsole();

    const char* inp_filename = "program.bin";
    int arg_filename = parseArg(argc, argv, "--input");
    if ( (arg_filename != ARG_NOT_FOUND) && (arg_filename + 1 < argc)) {
        inp_filename = argv[arg_filename + 1];
    }

    FILE* out_file = stdout;
    arg_filename = parseArg(argc, argv, "--output");
    if ( (arg_filename != ARG_NOT_FOUND) && (arg_filename + 1 < argc)) {
        out_file = fopen(argv[arg_filename + 1], "r");
        if(out_file == nullptr){
            error_log("Output file not found");
            return 1;
        }
    }


    size_t file_size = 0;
    uint8_t* input_data = (uint8_t*)readBinFile(inp_filename, &file_size);
    if(input_data == nullptr){
        return 1;
    }

    printf("Siganture: 0x%08X Version: %u\n", *(int*)input_data, *(input_data + sizeof(SIGNATURE)));


    uint8_t* ip = input_data + sizeof(SIGNATURE) + sizeof(VERSION);
    while(ip < input_data + file_size){
        int instr_status = 0;
        for (int i = 0; i < CMP_INSTR_COUNT; i++){
            if (CMP_INSTR_LIST[i].code == *ip){
                instr_status = 1;
                printf("%s ", CMP_INSTR_LIST[i].name);
                ip+=2;
                printInstrArg(stdout, *(ip-1), &ip);
                printf("\n");
                break;
            }
        }
        if(instr_status == 0){
            printf("[Bad instruction] Code %02X \n", ((*ip) & MASK_CMD_CODE));
            ip++;
        }
    }


}
