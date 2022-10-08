#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

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

    const char* inp_filename = "input.txt";
    int arg_filename = parseArg(argc, argv, "--input");
    if ( (arg_filename != ARG_NOT_FOUND) && (arg_filename + 1 < argc)) {
        inp_filename = argv[arg_filename + 1];
    }
    Text input_txt = readFileLines(inp_filename);
    if(input_txt.text_data == nullptr){
        return 1;
    }

    const char* out_filename = "program.bin";
    arg_filename = parseArg(argc, argv, "--output");
    if ( (arg_filename != ARG_NOT_FOUND) && (arg_filename + 1 < argc)) {
        out_filename = argv[arg_filename + 1];
    }

    Stack stk;
    stackCtor(&stk);


    void* output = calloc(input_txt.length*(sizeof(int) + sizeof(char)) + sizeof(SIGNATURE),1);

    void* output_ptr = output;

    *(uint32_t*)output_ptr = SIGNATURE;
    output_ptr = (uint32_t*)output_ptr + 1;

    *(uint16_t*)output_ptr = VERSION;
    output_ptr = (uint16_t*)output_ptr + 1;

    for (size_t ip = 0; ip < input_txt.length; ip++){
        const char* str = input_txt.lines[ip].chars;
        int instr_status = 0; //0 not found 1 ok -1 error
        for (int i = 0; i < INSTR_COUNT; i++){
            size_t t = stricmp_len(str, INSTR_LIST[i].name);
            if (!isgraph(str[t])){
                instr_status = 1;
                *(uint8_t*)output_ptr = INSTR_LIST[i].code;
                output_ptr = 1 + ((uint8_t*)output_ptr);

                if((sscanf(str + t, "%d", (int*) output_ptr) > 0) != INSTR_LIST[i].hasArg){
                    instr_status = -1;
                    setConsoleColor(stdout, COLOR_REDD , COLOR_BLACK);
                    printf("Error : instruction %s at line %d got incorrect argument\n", INSTR_LIST[i].name, ip);
                    setConsoleColor(stdout, COLOR_DEFAULTT, COLOR_BLACK);
                    break;
                }
                if(INSTR_LIST[i].hasArg)
                    output_ptr = 1 + ((int*)output_ptr);
                break;
            }
        }

        if(instr_status == 0){
            setConsoleColor(stdout, COLOR_REDD , COLOR_BLACK);
            printf("Error : Unknown instruction at line %d\n", ip);
            setConsoleColor(stdout, COLOR_DEFAULTT, COLOR_BLACK);
        }
        if(instr_status != 1)
            break;
    }
    deleteText(&input_txt);

    for(uint8_t* i = (uint8_t*)output; i < output_ptr; i++){
        printf("%02X ", *i);
    }

    FILE* out_file = fopen(out_filename, "wb");
    fwrite(output, 1, (uint8_t*)output_ptr - (uint8_t*)output, out_file);
    fclose(out_file);
    free(output);

    return 0;
}
