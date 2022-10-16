#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>


#include "lib\logging.h"
#include "lib\file_read.h"
#include "instr\instructions_compile.h"
#include "arg_parser.h"

static size_t stricmp_len(const char* str1, const char* str2) {
    const char* str1_start = str1;
    while (*str2 != '\0' && tolower(*str1) == tolower(*str2)) {
        str1++; str2++;
    }
    if(*str2 == '\0'){
        return str1 - str1_start;
    }
    else{
        return 0;
    }
}

uint8_t* asmCompile(const Text input_txt, size_t* size_ptr, bool stop_on_err){
    initLiteralReplaceArray();

    label* const  labels = (label*)calloc(input_txt.length, sizeof(label));
    label* const  labels_last = labels + input_txt.length - 1;

    label* label_place_ptr = labels;
    label* label_addr_ptr = labels_last;


    uint8_t* const output = (uint8_t*)calloc(input_txt.length*(sizeof(PROC_DATA_T) + 3) + sizeof(SIGNATURE),1);
    uint8_t* output_ptr = output;

    *(uint32_t*)output_ptr = SIGNATURE;
    output_ptr = output_ptr + sizeof(SIGNATURE);

    *(uint16_t*)output_ptr = VERSION;
    output_ptr = output_ptr + sizeof(VERSION);

    uint8_t* const out_prog_start = output_ptr;

    bool compilation_error = false;
    for (size_t ip = 0; ip < input_txt.length; ip++){
        const char* str = input_txt.lines[ip].chars;

        printf_log("[%08X] ", output_ptr - out_prog_start);
        while (isspace(*str)){
            str++;
            input_txt.lines[ip].length--;
        }

        if (*str == '\0' || *str == '#'){
            printf_log("[COMMENT]\n");
            continue;
        }
        if (str[input_txt.lines[ip].length-1] == ':'){
            label_addr_ptr->name = str;
            label_addr_ptr->addr = output_ptr;
            label_addr_ptr--;
            printf_log(">%s\n", str);
            continue;
        }
        int instr_status = 0; //0 not found 1 ok -1 error
        for (int i = 0; i < CMP_INSTR_COUNT; i++){
            size_t t = stricmp_len(str, CMP_INSTR_LIST[i].name);
            if (!isgraph(str[t])){
                instr_status = 1;
                *output_ptr = CMP_INSTR_LIST[i].code;
                output_ptr++;

                int arg_size = parseInstrArg(str + t, output_ptr, &label_place_ptr);

                printf_log("%6s (%02X) arg%X(+%d)\n",CMP_INSTR_LIST[i].name, CMP_INSTR_LIST[i].code, ((*output_ptr) & (~MASK_CMD_CODE)) >> 4, arg_size-1);
                if (arg_size == -1){
                    instr_status = -1;
                    printf("%s\n", str+t);
                    continue;
                }
                if (!matchesArgReq(*output_ptr, CMP_INSTR_LIST[i].arg_req)){
                    instr_status = -1;
                    error_log("Error : instruction %s at line %d got incorrect argument\n", CMP_INSTR_LIST[i].name, ip);
                    continue;
                }
                output_ptr += arg_size;
                break;
            }
        }

        if (instr_status != 1){
            error_log("Error : Unknown instruction %s at line %d\n", str, ip);
            compilation_error = true;
        }
        if (compilation_error && stop_on_err){
            break;
        }
    }

    if(compilation_error){
        free(output);
        free(labels);
        error_log("Assembly errors occured. Output not created\n");
        return nullptr;
    }

    for (label* j = labels; j < label_place_ptr; j++){
        bool found_lbl = false;
        for (label* i = labels_last; i > label_addr_ptr; i--){
            size_t t = stricmp_len(i->name, j->name);
            if (i->name[t] == ':' && (j->name[t] == ' ' || j->name[t] == '\0' || j->name[t] == '+')){
                if(found_lbl){
                    error_log("Multiple definitions of label %s found\n", i->name);
                    compilation_error = true;
                    if(stop_on_err){
                        break;
                    }
                }
                info_log("add label %s (%X) to constant at %X\n", i->name, i->addr - out_prog_start , j->addr - out_prog_start);
                *((PROC_DATA_T*)j->addr) += i->addr - out_prog_start;
                found_lbl = true;
            }
        }
        if(!found_lbl){
            compilation_error = true;
            error_log("Label %s undefined\n", j->name);
        }
        if (compilation_error && stop_on_err){
            break;
        }
    }
    if(compilation_error){
        free(output);
        free(labels);
        error_log("Assembly errors occured. Output not created\n");
        return nullptr;
    }

    free(labels);
    *size_ptr = output_ptr - output;
    return output;
}
