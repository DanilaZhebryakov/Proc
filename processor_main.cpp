#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "Console_utils.h"
#include "instructions.h"
#include "processor.h"
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

    if (*(uint32_t*)program_data != SIGNATURE){
        Error_log("Error : input program signature bad. Expected %X got %X\n", SIGNATURE, *(uint32_t*)program_data);
        free(input_data);
        return EXIT_FAILURE;
    }
    program_data += sizeof(SIGNATURE);

    if (*(uint16_t*)program_data != VERSION){
        Error_log("Error : input program version bad Expected %n got %n\n", VERSION, *(uint16_t*)program_data);
        free(input_data);
        return EXIT_FAILURE;
    }
    program_data += sizeof(VERSION);

    printf_log("\n");

    Processor proc = {prog_size - sizeof(VERSION) - sizeof(SIGNATURE), program_data, program_data, &stk};
    proc.ram_size = 100;
    proc.ram = (int*)calloc(proc.ram_size, sizeof(int));

    procError_t err = procRunCode(&proc);
    printProcError(err);


    programDump(&proc);

    stackDtor(&stk);
    free(input_data);
    free(proc.ram);

    return 0;
}
