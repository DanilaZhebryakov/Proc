#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>


#include "lib\logging.h"
#include "lib\file_read.h"
#include "instr\instructions_compile.h"

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

static char literal_replaces[256];

void initLiteralReplaceArray(){
    literal_replaces['n'] = '\n';
    literal_replaces['a'] = '\a';
    literal_replaces['r'] = '\r';
    literal_replaces['0'] = '\0';

    literal_replaces['\\'] = '\\';
    literal_replaces['\''] = '\'';
}


#define skipSpaces(_str) \
    while(isspace(*_str)){ \
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

#define nextArgOrEnd(_str)   \
    skipSpaces(_str);        \
    checkForLineEnd(_str)    \
    if(*_str != '+'){        \
        error_log("no + and no line end after argument\n"); \
        return -1;           \
    }                        \
    _str++;                  \
    skipSpaces(_str);        \

int xdigit2int(char c){
    if(c >= '0' && c <= '9'){
        return c - '0';
    }
    if(c >= 'a' && c <= 'f'){
        return 10 + c - 'a';
    }
    if(c >= 'A' && c <= 'F'){
        return 10 + c - 'A';
    }
}

PROC_DATA_T parseInstrArg(const char* str, uint8_t* out, label** lbl){
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
            if(*str >= 'a' && *str <= 'z' && *(str+1) == 'x'){
                *out = (*str - 'a' + 1);
                str += 2;
                out++;
            }
            else return -1;
        }
        nextArgOrEnd(str)
    }

    if (*str == ':'){
        str++; //:
        *out_beg |= MASK_CMD_IMM;
        (*lbl)->name = str;
        (*lbl)->addr = out;
        (*lbl)++;

        *(PROC_DATA_T*)out  = 0;
        out += sizeof(PROC_DATA_T);

        while(*str != ']' && *str != '\0' && *str != '+'){
            str++;
        }
        nextArgOrEnd(str)
        out -= sizeof(PROC_DATA_T); //need to overwrite the constant part
    }

    if (*str == '\''){
        str++; //'
        *out_beg |= MASK_CMD_IMM;
        if(*str == '\\'){
            str++;
            *(PROC_DATA_T*)out  = literal_replaces[*str];
        }
        else{
            *(PROC_DATA_T*)out  = *str;
        }
        out += sizeof(PROC_DATA_T);
        str++;
        if(*str != '\''){
            error_log("no closing ' on char literal\n");
            return -1;
        }
        str++;
        nextArgOrEnd(str)

        out -= sizeof(PROC_DATA_T);
    }

    if (!(isdigit(*str) || *str == '-')){
        error_log("no valid arg found\n");
        return -1;
    }

    *out_beg |= MASK_CMD_IMM;
    int n = 0;
    bool is_neg = (*str == '-');
    PROC_DATA_T val = 0;
    if (is_neg){
        str++;
    }
    if (*str == '0'){
        str++;
        if(*str == 'x'){
            str++;
            while(isxdigit(*str)){
                val  = val << 4;
                val |= xdigit2int(*str);
                str++;
            }
        }
        else{
            while(*str >= '0' && *str <= '7'){
                val  = val << 3;
                val |= *str - '0';
                str++;
            }
        }
    }
    else{
        sscanf(str, PROC_DATA_SPEC "%n", &val, &n);
    }
    if (is_neg){
        val = -val;
    }
    *(PROC_DATA_T*)out = val;
    str += n;
    out += sizeof(PROC_DATA_T);

    checkForLineEnd(str);
    printf("!%s %d \n", str, val);
    error_log("no line end after const argument\n");
    return -1;
}


uint8_t* asmCompile(const Text input_txt, size_t* size_ptr){
    initLiteralReplaceArray();

    label* const  labels = (label*)calloc(input_txt.length, sizeof(label));
    label* const  labels_last = labels + input_txt.length - 1;

    label* label_place_ptr = labels;
    label* label_addr_ptr = labels_last;


    uint8_t* const output = (uint8_t*)calloc(input_txt.length*(sizeof(PROC_DATA_T) + 2) + sizeof(SIGNATURE),1);
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
        for (int i = 0; i < CMP_INSTR_COUNT; i++){
            size_t t = stricmp_len(str, CMP_INSTR_LIST[i].name);
            if (!isgraph(str[t])){
                instr_status = 1;
                *output_ptr = CMP_INSTR_LIST[i].code;

                int arg_size = parseInstrArg(str + t, output_ptr, &label_place_ptr);

                printf_log("%6s (%02X) arg%X(+%d)\n",CMP_INSTR_LIST[i].name, CMP_INSTR_LIST[i].code, ((*output_ptr) & (~MASK_CMD_CODE)) >> 4, arg_size-1);
                if (arg_size == -1){
                    printf("%s\n", str+t);
                    free(output);
                    free(labels);
                    return nullptr;
                }
                if (!matchesArgReq(*output_ptr, CMP_INSTR_LIST[i].arg_req)){
                    instr_status = -1;
                    error_log("Error : instruction %s at line %d got incorrect argument\n", CMP_INSTR_LIST[i].name, ip);
                    free(output);
                    free(labels);
                    return nullptr;
                }
                output_ptr += arg_size;
                break;
            }
        }

        if (instr_status == 0){
            error_log("Error : Unknown instruction %s at line %d\n", str, ip);
        }
        if (instr_status != 1)
            break;
    }

    for (label* i = labels_last; i > label_addr_ptr; i--){
        for (label* j = labels; j < label_place_ptr; j++){
            size_t t = stricmp_len(i->name, j->name);
            if (i->name[t] == ':' && (j->name[t] == ' ' || j->name[t] == '\0' || j->name[t] == '+')){
                info_log("add label %s (%X) to constant at %X\n", i->name, i->addr - out_prog_start , j->addr - out_prog_start);
                *((PROC_DATA_T*)j->addr) += i->addr - out_prog_start;
            }
        }
    }

    free(labels);
    *size_ptr = output_ptr - output;
    return output;
}
