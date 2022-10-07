#include <stdio.h>
#include <stdlib.h>

#include "Console_utils.h"
#include "instructions.h"
#include "file_read.h"
#include "parseArg.h"

size_t stricmp_len(const char* str1, const char* str2){
    const char* str1_start = str1;
    while(*str1 != '\0' && tolower(*str1) == tolower(*str2)){
        str1++; str2++;
    }
    return str1 - str1_start;
}

int main(int argc, const char *argv[]) {
    initConsole();

    const char* inp_filename = "program.bin";
    int arg_filename = parseArg(argc, argv, "--input");
    if ( (arg_filename != ARG_NOT_FOUND) && (arg_filename + 1 < argc)) {
        inp_filename = argv[arg_filename + 1];
    }

    Stack stk;
    stackCtor(&stk);
    size_t prog_size = 0;
    uint8_t* input_data = (uint8_t*)readBinFile(inp_filename, &prog_size);

    uint8_t* program_data = input_data;

    if(*(uint32_t*)program_data != SIGNATURE){
        error_log("Error : input program signature bad. Expected %X got %X\n", SIGNATURE, *(uint32_t*)program_data);
        free(input_data);
        return EXIT_FAILURE;
    }
    program_data += sizeof(SIGNATURE);

    if(*(uint16_t*)program_data != VERSION){
        error_log("Error : input program version bad Expected %n got %n\n", VERSION, *(uint16_t*)program_data);
        free(input_data);
        return EXIT_FAILURE;
    }
    program_data += sizeof(VERSION);


    Processor proc = {prog_size - sizeof(VERSION) - sizeof(SIGNATURE), program_data, program_data};
    while(proc.ip - proc.data < proc.prog_size){
        const uint8_t instr_code = *(proc.ip);
        proc.ip++;

        info_log("Instr: %X ", instr_code);
        procError_t err = PROC_BADCMD;
        for (int i = 0; i < INSTR_COUNT; i++){
            if (instr_code == INSTR_LIST[i].code){
                printf_log("(%s)", INSTR_LIST[i].name);
                err = INSTR_LIST[i].func(&stk, &proc);
                break;
            }
        }
        printf_log("\n");

        if (err != PROC_NOERROR){
            printf("Program stopped\n");
            printf("ip = %d\n", proc.ip - proc.data);
            if (err & PROC_HALT)
                printf(" Halted\n");
            if (err & PROC_ERRUNK)
                printf(" Unknown error\n");
            if (err & PROC_BADCMD)
                printf(" Instruction not found\n");
            if (err & PROC_STACKEMPT)
                printf(" Not enough elements in stack\n");
            if (err & PROC_INT_ERROR)
                printf(" Internal error\n");
            if (err & PROC_DIV0)
                printf(" Zero division error\n");
            break;
        }
    }
    stackDump(&stk);
    programDump(&proc);

    stackDtor(&stk);
    free(input_data);

    return 0;
}
