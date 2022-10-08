#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "instructions.h"
#include "file_read.h"

static size_t stricmp_len(const char* str1, const char* str2){
    const char* str1_start = str1;
    while(*str1 != '\0' && tolower(*str1) == tolower(*str2)){
        str1++; str2++;
    }
    return str1 - str1_start;
}

uint8_t* asmCompile(Text input_txt, size_t* size_ptr){
    uint8_t* output = (uint8_t*)calloc(input_txt.length*(sizeof(int) + sizeof(char)) + sizeof(SIGNATURE),1);

    uint8_t* output_ptr = output;

    *(uint32_t*)output_ptr = SIGNATURE;
    output_ptr = output_ptr + sizeof(SIGNATURE);

    *(uint16_t*)output_ptr = VERSION;
    output_ptr = output_ptr + sizeof(VERSION);

    for (size_t ip = 0; ip < input_txt.length; ip++){
        const char* str = input_txt.lines[ip].chars;
        int instr_status = 0; //0 not found 1 ok -1 error
        for (int i = 0; i < INSTR_COUNT; i++){
            size_t t = stricmp_len(str, INSTR_LIST[i].name);
            if (!isgraph(str[t])){
                instr_status = 1;
                *output_ptr = INSTR_LIST[i].code;
                output_ptr++;

                if((sscanf(str + t, "%d", (int*)output_ptr) > 0) != INSTR_LIST[i].hasArg){
                    instr_status = -1;
                    setConsoleColor(stdout, COLOR_REDD , COLOR_BLACK);
                    printf("Error : instruction %s at line %d got incorrect argument\n", INSTR_LIST[i].name, ip);
                    setConsoleColor(stdout, COLOR_DEFAULTT, COLOR_BLACK);
                    break;
                }
                if(INSTR_LIST[i].hasArg)
                    output_ptr += sizeof(int);
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
    *size_ptr = output_ptr - output;
    return output;
}
