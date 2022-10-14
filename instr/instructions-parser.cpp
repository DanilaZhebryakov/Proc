#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "lib\file_read.h"
static const char* const INSTR_FUNC_PREFIX      = "static procError_t ";
static const char* const INSTR_FUNC_NAME_PREFIX = "instr";
static const char* const INSTR_FUNC_ARGS        = " # (Processor* # ) ";

static const char* const INSTR_FUNC_READ  = "getInstrArg(";
static const char* const INSTR_FUNC_WRITE = "setInstrArg(";

const uint8_t MAX_INSTRUCTION_CODE = 0x1F;

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
    return (isspace(c) || ispunct(c) || iscntrl(c)) && (c != '_' && c != '-');
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

static void printUntilSep(FILE* file, const char* str){
    while (!isProgramSeparator(*str)){
        putc(*str, file);
        str++;
    }
}

static void printAlpha(FILE* file, const char* str){
    while (isalpha(*str)){
        putc(*str, file);
        str++;
    }
}

struct InstructionEntry{
    const char* func_name;
    const char* name;
    bool isRead;
    bool isWrite;
};

int main(){

    int cmp_code = system("g++ instructions.h -I .. -I ../lib -o instructions.txt -E");
    printf("Preprocessor exit code: %d\n" , cmp_code);
    if (cmp_code != 0){
        return EXIT_FAILURE;
    }

    FILE* instr_txt = fopen(TEMP_FILE_NAME, "r");
    if (instr_txt == nullptr){
        return EXIT_FAILURE;
    }
    String str = readFile(instr_txt);
    if (str.chars == nullptr){
        return EXIT_FAILURE;
    }
    fclose(instr_txt);
    system("rm " TEMP_FILE_NAME);

    bool is_in_str = false;
    bool is_in_chr = false;
    int blockLvl = 0;

    char* instr_start       = nullptr;
    const char* instr_name = nullptr;
    bool isRead  = false;
    bool isWrite = false;
    short unsigned int instr_code = 0; // Warning: scanf
    uint8_t instr_code_space = 0;

    InstructionEntry* instructions = (InstructionEntry*)calloc(MAX_INSTRUCTION_CODE + 1, sizeof(InstructionEntry));

    for (char* i = str.chars; i - str.chars < str.length; i++){
        if (*i == '\\'){
            i++;
        }
        if (is_in_str){
            if (*i == '"')
                is_in_str = false;
            continue;
        }
        if (is_in_chr){
            if (*i == '\'')
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
                if ((instructions + instr_code)->name != nullptr)
                    *(instructions + instr_code_space) = *(instructions + instr_code);
                InstructionEntry* instr_ptr = instructions + instr_code;
                instr_ptr->func_name   = instr_name;
                instr_ptr->name        = instr_name + strlen(INSTR_FUNC_NAME_PREFIX);
                instr_ptr->isRead      = isRead;
                instr_ptr->isWrite     = isWrite;

                while((instructions + instr_code_space)->name != nullptr)
                    instr_code_space++;


                printUntilSep(stdout, instr_name);
                printf(" A:%02X R:%d W:%d L:%d\n", instr_code, isRead, isWrite, i - instr_start);
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
                if (instr_code_space > MAX_INSTRUCTION_CODE){
                    printf("[ERROR] Too many instructions!");
                    break;
                }
                instr_code = instr_code_space;
                sscanf(i, " %*[A-z]%hX", &instr_code);
                if (instr_code > MAX_INSTRUCTION_CODE){
                    printf("[ERROR] Bad instruction code %02X for func ");
                    printUntilSep(stdout, i);
                    printf("\n");
                    break;
                }
                i += t;
                instr_start = i;
                isRead  = false;
                isWrite = false;
            }
        }
    }



    FILE* out_file = fopen("instructions_proc.cpp", "w");
    fprintf(out_file,
           "#include <stdio.h>\n"
           "#include \"instructions_proc.h\"\n"
           "#include \"instructions.h\"\n"
           "// WARNING: auto-generated!\n"
           "//DO NOT CHANGE\n\n"
           "const struct Instruction PROC_INSTR_LIST[] = {\n");
    for (InstructionEntry* i = instructions; i <= instructions + MAX_INSTRUCTION_CODE; i++){

        fprintf(out_file, "{0x%02X", i - instructions);

        if( i->name != nullptr){
            fprintf(out_file, ", ");
            printUntilSep(out_file, i->func_name);
            fprintf(out_file, ", \"");
            printAlpha(out_file, i->name);
            fprintf(out_file, "\"");
            if      (i->isWrite){
                fprintf(out_file, ", ARG_WRITE");
            }
            else if (i->isRead){
                fprintf(out_file, ", ARG_READ");
            }
        }
        fprintf(out_file, "},\n");
    }
    fprintf(out_file, "};\n");
    fprintf(out_file, "const int PROC_INSTR_COUNT = sizeof(PROC_INSTR_LIST) / sizeof(Instruction);\n");

    fclose(out_file);


    out_file = fopen("instructions_compile.cpp", "w");
    fprintf(out_file,
           "#include <stdio.h>\n"
           "#include \"instructions_compile.h\"\n"
           "// WARNING: auto-generated!\n"
           "//DO NOT CHANGE\n\n"
           "const struct CompileInstruction CMP_INSTR_LIST[] = {\n");

    for (InstructionEntry* i = instructions; i < instructions + MAX_INSTRUCTION_CODE; i++){
        if( i->name == nullptr){
            continue;
        }
        fprintf(out_file, "{0x%02X, ", i - instructions);

        fprintf(out_file, "\"");
        printAlpha(out_file, i->name);
        fprintf(out_file, "\"");
        if      (i->isWrite){
            fprintf(out_file, ", ARG_WRITE");
        }
        else if (i->isRead){
            fprintf(out_file, ", ARG_READ");
        }
        fprintf(out_file, "},\n");
    }
    fprintf(out_file, "};\n");
    fprintf(out_file, "const int CMP_INSTR_COUNT = sizeof(CMP_INSTR_LIST) / sizeof(CompileInstruction);\n");

    fclose(out_file);


    free(str.chars);
    nullifyString(&str);
}