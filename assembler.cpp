#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "instructions.h"
#include "file_read.h"

struct label{
    const char* name;
    uint8_t* addr;
};

static size_t stricmp_len(const char* str1, const char* str2) {
    const char* str1_start = str1;
    while (*str1 != '\0' && tolower(*str1) == tolower(*str2)) {
        str1++; str2++;
    }
    return str1 - str1_start;
}

#define skipSpaces(_str) \
    while(*_str == ' '){ \
        _str++;          \
    }


#define checkForLineEnd(_str) \
    if(*_str == '\0' || *_str == ']'){ \
        if((*_str == ']') != ((*out_beg & MASK_CMD_MEM) != 0)){ \
            error_log("memory [] not match\n"); \
            return -1; \
        } \
        if((*_str == ']') && (*(_str + 1) != '\0')){ \
            error_log("text after closing ]\n"); \
            return -1; \
        } \
        return out-out_beg; \
    }


int parseInstrArg(const char* str, uint8_t* out, label** lbl){
    const char* str_beg = str;
    uint8_t* out_beg = out;
    out++;

    skipSpaces(str);
    if(*str == '\0'){
        return 1;
    }
    if(*str == '[') {
        *out_beg |= MASK_CMD_MEM;
        str++; //[
    }
    skipSpaces(str);

    if(*str == 'r'){
        *out_beg |= MASK_CMD_REG;
        str++; //r
        if(isdigit(*str)){
            int n = 0;
            sscanf(str, "%d%n", out, &n);
            str += n; //XX
            out++;
        }
        else{
            return -1; // add rXx support here at some point
        }
        skipSpaces(str);
        checkForLineEnd(str)
        if(*str != '+'){
            error_log("no + and no line end after register argument\n");
            return -1;
        }
        str++; //+
        skipSpaces(str);
    }

    if (*str == ':'){
        str++; //:
        *out_beg |= MASK_CMD_IMM;
        (*lbl)->name = str;
        (*lbl)->addr = out;
        (*lbl)++;

        *(int*)out  = 0;
        out += sizeof(int);

        while(*str != ']' && *str != '\0' && *str != '+'){
            str++;
        }
        checkForLineEnd(str);
        skipSpaces(str);
        if(*str != '+'){
            error_log("no + and no line end after label argument\n");
            return -1;
        }
        str++; //+
        skipSpaces(str);
        out -= sizeof(int); //need to overwrite the constant part
    }

    if (!(isdigit(*str) || *str == '-')){
        error_log("no const arg and no reg arg found\n");
        return -1;
    }
    *out_beg |= MASK_CMD_IMM;
    int n = 0;
    sscanf(str, "%d%n", (int*)out, &n);
    str += n;
    out += sizeof(int);

    checkForLineEnd(str);
    error_log("no line end after const argument\n");
    return -1;
}


uint8_t* asmCompile(const Text input_txt, size_t* size_ptr){
    label* const  labels = (label*)calloc(input_txt.length, sizeof(label));
    label* const  labels_last = labels + input_txt.length - 1;

    label* label_place_ptr = labels;
    label* label_addr_ptr = labels_last;


    uint8_t* const output = (uint8_t*)calloc(input_txt.length*(sizeof(int) + 2) + sizeof(SIGNATURE),1);

    uint8_t* output_ptr = output;

    *(uint32_t*)output_ptr = SIGNATURE;
    output_ptr = output_ptr + sizeof(SIGNATURE);

    *(uint16_t*)output_ptr = VERSION;
    output_ptr = output_ptr + sizeof(VERSION);

    uint8_t* const out_prog_start = output_ptr;

    for (size_t ip = 0; ip < input_txt.length; ip++){
        const char* str = input_txt.lines[ip].chars;

        printf_log("[%08X] ", output_ptr - out_prog_start);

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
        for (int i = 0; i < INSTR_COUNT; i++){
            size_t t = stricmp_len(str, INSTR_LIST[i].name);
            if (!isgraph(str[t])){
                instr_status = 1;
                *output_ptr = INSTR_LIST[i].code;

                int arg_size = parseInstrArg(str + t, output_ptr, &label_place_ptr);

                printf_log("%6s (%02X) arg%X(+%d)\n",INSTR_LIST[i].name, INSTR_LIST[i].code, ((*output_ptr) & (~MASK_CMD_CODE)) >> 4, arg_size-1);
                if (arg_size == -1){
                    printf("%s\n", str+t);
                    free(output);
                    free(labels);
                    return nullptr;
                }
                if (!matchesArgReq(*output_ptr, INSTR_LIST[i].arg_req)){
                    instr_status = -1;
                    error_log("Error : instruction %s at line %d got incorrect argument\n", INSTR_LIST[i].name, ip);
                    free(output);
                    free(labels);
                    return nullptr;
                }
                output_ptr += arg_size;
                break;
            }
        }

        if (instr_status == 0){
            error_log("Error : Unknown instruction at line %d\n", ip);
        }
        if (instr_status != 1)
            break;
    }

    for (label* i = labels_last; i > label_addr_ptr; i--){
        for (label* j = labels; j < label_place_ptr; j++){
            size_t t = stricmp_len(i->name, j->name);
            if (i->name[t] == ':' && (j->name[t] == ' ' || j->name[t] == '\0' || j->name[t] == '+')){
                info_log("add label %s (%X) to constant at %X\n", i->name, i->addr - out_prog_start , j->addr - out_prog_start);
                *((int*)j->addr) += i->addr - out_prog_start;
            }
        }
    }

    free(labels);
    *size_ptr = output_ptr - output;
    return output;
}
