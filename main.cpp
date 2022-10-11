#include <stdio.h>
#include <stdlib.h>


#include "Console_utils.h"
#include "instructions_lib.h"
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

    const char* inp_filename = "input.txt";
    int arg_filename = parseArg(argc, argv, "--input");
    if ( (arg_filename != ARG_NOT_FOUND) && (arg_filename + 1 < argc)) {
        inp_filename = argv[arg_filename + 1];
    }
    Text input_txt = readFileLines(inp_filename);

    if(input_txt.text_data == nullptr){
        return 0;
    }

    initConsole();


    Stack stk;
    stackCtor(&stk);


    Processor program = {input_txt.length, &input_txt, 0, nullptr};
    for (program.ip = 0; program.ip < program.size; program.ip++){
        const char* str = program.data->lines[program.ip].chars;
        procError_t err = PROC_BADCMD;
        for (int i = 0; i < INSTR_COUNT; i++){
            size_t t = stricmp_len(str, INSTR_LIST[i].name);
            if (!isgraph(str[t])){
                program.arg_ptr = str + t;
                err = INSTR_LIST[i].func(&stk, &program);
                break;
            }
        }
        if (err != PROC_NOERROR){
            printf("Program stopped\n");
            printf("ip = %d\n", program.ip);
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
    programDump(&program);

    stackDtor(&stk);
    deleteText(&input_txt);

    return 0;
}
