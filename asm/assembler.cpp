#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>


#include "lib\logging.h"
#include "lib\file_read.h"
#include "instructions_asm.h"
#include "arg_parser.h"

static size_t stricmp_len(const char* str1, const char* str2) {
    const char* str1_start = str1;
    while (*str2 != '\0' && tolower(*str1) == tolower(*str2)) {
        str1++; str2++;
    }
    return str1 - str1_start;
}

static bool compileInstruction(const char* str, uint8_t** output_ptr, size_t ip, label**  label_place_ptr){
    for (int i = 0; i < CMP_INSTR_COUNT; i++){
        size_t scl = stricmp_len(str, CMP_INSTR_LIST[i].name);
        if (!isgraph(str[scl]) && CMP_INSTR_LIST[i].name[scl] == '\0'){
            **output_ptr = CMP_INSTR_LIST[i].code;
            *output_ptr += 1;

            int arg_size = parseInstrArg(str + scl, *output_ptr, label_place_ptr);

            printf_log("%6s (%02X) arg%X(+%d)\n",CMP_INSTR_LIST[i].name, CMP_INSTR_LIST[i].code, ((**output_ptr) & (~MASK_CMD_CODE)) >> 4, arg_size-1);
            if (arg_size == -1){
                printf_log("%s\n", str+scl);
                return false;
            }
            if (!matchesArgReq(**output_ptr, CMP_INSTR_LIST[i].arg_req)){
                error_log("Error : instruction %s at line %d got incorrect argument\n", CMP_INSTR_LIST[i].name, ip);
                return false;
            }
            *output_ptr += arg_size;
            return true;
        }
    }

    error_log("Error : Unknown instruction %s at line %d\n", str, ip);
    return false;
}

bool placeLabels(label* lbl_place_b, label* lbl_place_e, label* lbl_addr_b, label* lbl_addr_e, uint8_t* addr_origin, bool stop_on_err){
    bool compilation_error = false;
    for (label* l_place = lbl_place_b; l_place < lbl_place_e; l_place++){
        bool found_lbl = false;
        for (label* l_def = lbl_addr_b; l_def < lbl_addr_e; l_def++){
            size_t scl = stricmp_len(l_def->name, l_place->name);
            if (l_def->name[scl] == ':' && (l_place->name[scl] == '\0' || strchr(" +-", l_place->name[scl]))){
                if(found_lbl){
                    error_log("Multiple definitions of label %s found\n", l_def->name);
                    compilation_error = true;
                    if(stop_on_err){
                        return false;
                    }
                }
                info_log("add label %s (%X) to constant at %X\n", l_def->name, l_def->addr - addr_origin , l_place->addr - addr_origin);
                *((PROC_DATA_T*)l_place->addr) += l_def->addr - addr_origin;
                found_lbl = true;
            }
        }
        if(!found_lbl){
            compilation_error = true;
            error_log("Label %s undefined\n", l_place->name);
        }
        if (compilation_error && stop_on_err){
            return false;
        }
    }
    return !compilation_error;
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
    output_ptr += sizeof(SIGNATURE);

    *(uint16_t*)output_ptr = VERSION;
    output_ptr += sizeof(VERSION);

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
        if(!compileInstruction(str, &output_ptr, ip, &label_place_ptr)){
            compilation_error = true;
            if (compilation_error && stop_on_err){
                break;
            }
        }
    }

    if(compilation_error){
        free(output);
        free(labels);
        error_log("Assembly errors occured. Output not created\n");
        return nullptr;
    }

    if(!placeLabels(labels, label_place_ptr, label_addr_ptr + 1, labels_last + 1, out_prog_start, stop_on_err)){
        free(output);
        free(labels);
        error_log("Assembly errors occured. Output not created\n");
        return nullptr;
    }

    free(labels);
    *size_ptr = output_ptr - output;
    return output;
}
