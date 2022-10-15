#include <ctype.h>

#include "arg_parser.h"

static char literal_replaces[256];

void initLiteralReplaceArray(){
    literal_replaces['n'] = '\n';
    literal_replaces['a'] = '\a';
    literal_replaces['r'] = '\r';
    literal_replaces['0'] = '\0';

    literal_replaces['\\'] = '\\';
    literal_replaces['\''] = '\'';
}

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
    if(*_str != '+' && *_str != '-'){        \
        error_log("no + and no line end after argument\n"); \
        return -1;           \
    }                        \
    is_neg = (*_str == '-');    \
    _str++;                  \
    skipSpaces(_str);        \

int parseCharLiteral(const char* str, uint8_t* out, label** lbl, bool neg){
    const char* str_beg = str;
    str++; //'
    PROC_DATA_T val = 0;
    if(*str == '\\'){
        str++;
        val = literal_replaces[*str];
    }
    else{
        val = *str;
    }
    str++;
    if(*str != '\''){
        error_log("no closing ' on char literal\n");
        return -1;
    }
    str++;
    if(neg){
        val = -val;
    }
    *(PROC_DATA_T*)out  = val;

    return str - str_beg;
}

int parseNumberLiteral(const char* str, uint8_t* out, label** lbl, bool neg){
    PROC_DATA_T val = 0;
    int n = 0;
    const char* str_beg = str;
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
    if(neg){
        val = -val;
    }

    *(PROC_DATA_T*)out = val;
    str += n;
    return str - str_beg;
}

int parseLabelLiteral(const char* str, uint8_t* out, label** lbl, bool neg){
    if(neg){
        error_log("Subtraction of label values not implemented");
    }


    const char* str_beg = str;
    str++; //:
    (*lbl)->name = str;
    (*lbl)->addr = out;
    (*lbl)++;

    *(PROC_DATA_T*)out  = 0;

    while(*str != ']' && *str != '\0' && *str != '+'){
        str++;
    }

    return str - str_beg;
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

    bool is_neg = false;
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
    *out_beg |= MASK_CMD_IMM;
    out += sizeof(PROC_DATA_T);
    while(1){
        int t = 0;
        skipSpaces(str)
        switch(*str){
            case '-':
                is_neg = true;
                str++;
            break;

            case ':':
                t = parseLabelLiteral(str, out - sizeof(PROC_DATA_T), lbl, is_neg);
                if(t == -1){
                    return -1;
                }
                str += t;
                nextArgOrEnd(str)
                break;
            case '\'':
                t = parseCharLiteral(str,  out - sizeof(PROC_DATA_T), lbl, is_neg);
                if(t == -1){
                    return -1;
                }
                str += t;
                nextArgOrEnd(str)
                break;
            default:
                if (!isdigit(*str)){
                    error_log("no valid arg found\n");
                    return -1;
                }
                t = parseNumberLiteral(str,  out - sizeof(PROC_DATA_T), lbl, is_neg);
                if(t == -1){
                    return -1;
                }
                str += t;
                nextArgOrEnd(str)
                break;
        }

    }
    return -1;
}
