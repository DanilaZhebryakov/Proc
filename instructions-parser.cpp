#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "file_read.h"
static const char* const INSTR_FUNC_PREFIX      = "static procError_t ";
static const char* const INSTR_FUNC_NAME_PREFIX = "instr";
static const char* const INSTR_FUNC_ARGS        = " # (Processor* # ) ";

static const char* const INSTR_FUNC_READ  = "getInstrArg(";
static const char* const INSTR_FUNC_WRITE = "setInstrArg(";

#define TEMP_FILE_NAME "instructions.txt"



#define skipSeparators(_str) \
    while(isProgramSeparator(*_str)){ \
        _str++;          \
    }

#define skipSpaces(_str) \
    while(*_str == ' '){ \
        _str++;          \
    }

static bool isProgramSeparator(char c){
    return isspace(c) || ispunct(c) || iscntrl(c);
}

static bool strStartsWith(const char* s1, const char* s2){
    while (*s2 != '\0' && *s1 == *s2){
        s1++;s2++;
    }
    return *s2 == '\0';
}

size_t strcmp_prog(const char* s1, const char* s2){
    if (*s1 == '\0'){
        return 0;
    }
    const char* s1_beg = s1;
    s1--;
    s2--;
    do {

        s1++; s2++;
        skipSpaces(s1);
        skipSpaces(s2);
        if (*s2 == '#') {
            s2++;
            while (*s1 != '\0' && !isProgramSeparator(*s1)) {
                s1++;
            }
            skipSpaces(s1);
            skipSpaces(s2);
        }
    } while (*s2 != '\0' && *s1 == *s2);

    if (*s2 == '\0')
        return s1 - s1_beg - 1;
    else
        return 0;
}

static void printFuncName(FILE* file, const char* str){
    while (!isProgramSeparator(*str)){
        putc(*str, file);
        str++;
    }
}


int main(){

    int cmp_code = system("g++ instructions.h -o "TEMP_FILE_NAME" -E");
    printf("Preprocessor exit code: %d\n" , cmp_code);

    FILE* instr_txt = fopen(TEMP_FILE_NAME, "r");
    String str = readFile(instr_txt);
    fclose(instr_txt);
    system("rm "TEMP_FILE_NAME);

    bool is_in_str = false;
    bool is_in_chr = false;
    int blockLvl = 0;

    char* instr_start       = nullptr;
    const char* instr_name = nullptr;
    bool isRead  = false;
    bool isWrite = false;
    uint8_t instrAddr = 0;

    FILE* out_file = fopen("instructions_array.cpp", "w");
    fprintf(out_file,
           "#include <stdio.h>\n"
           "#include \"instructions_lib.h\"\n"
           "#include \"instructions.h\"\n"
           "// WARNING: auto-generated!\n"
           "//DO NOT CHANGE\n\n"
           "const struct Instruction INSTR_LIST[] = {\n");


    for (char* i = str.chars; i - str.chars < str.length; i++){
        if (*i == '\\'){
            i++;
        }
        if (is_in_str){
            if(*i == '"')
                is_in_str = false;
            continue;
        }
        if (is_in_chr){
            if(*i == '\'')
                is_in_chr = false;
            continue;
        }

        if (*i == '"'){
            is_in_str = true;
            continue;
        }
        if (*i == '\''){
            is_in_chr = true;
            continue;
        }

        if (*i == '{'){
            blockLvl++;
        }
        if (*i == '}'){
            blockLvl--;
        }

        if (instr_start != nullptr){
            size_t t = strcmp_prog(i, INSTR_FUNC_READ);
            if (t != 0){
                isRead = true;
            }
            t = strcmp_prog(i, INSTR_FUNC_WRITE);
            if (t != 0){
                isWrite = true;
            }
            if (blockLvl == 0){
                if (instrAddr > 0x1F){
                    printf("[ERROR] Too many instructions!");
                    break;
                }
                fprintf(out_file, "{0x%02X, ", instrAddr++);
                printFuncName(out_file, instr_name);
                fprintf(out_file, ", \"");
                printFuncName(out_file, instr_name + strlen(INSTR_FUNC_NAME_PREFIX));
                fprintf(out_file, "\"");
                if (isWrite){
                    fprintf(out_file, ", ARG_WRITE");
                }
                else if(isRead){
                    fprintf(out_file, ", ARG_READ");
                }
                fprintf(out_file, "},\n");
                printFuncName(stdout, instr_name);
                printf(" R:%d W:%d L:%d\n", isRead, isWrite, i - instr_start);
                instr_start = nullptr;
            }
        }
        if (blockLvl != 0)
            continue;

        size_t t = strcmp_prog(i, INSTR_FUNC_PREFIX);
        if (t != 0){
            i += t;
            skipSeparators(i);
            instr_name = i;
            t = strcmp_prog(i, INSTR_FUNC_ARGS);
            if (t != 0 && strStartsWith(i, INSTR_FUNC_NAME_PREFIX)){

                i += t;
                instr_start = i;
                isRead  = false;
                isWrite = false;
            }
        }
    }
    fprintf(out_file, "};\n");
    fprintf(out_file, "const int INSTR_COUNT = sizeof(INSTR_LIST) / sizeof(Instruction);\n");

    fclose(out_file);
}
