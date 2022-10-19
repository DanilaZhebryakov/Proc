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

const uint8_t MAX_INSTRUCTION_CODE = 0xFF;

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
    if(str == nullptr){
        fputs("nullptr", file);
        return;
    }

    while (!isProgramSeparator(*str)){
        putc(*str, file);
        str++;
    }
}

static void printAlpha(FILE* file, const char* str){
    if(str == nullptr){
        return;
    }
    while (isalpha(*str)){
        putc(*str, file);
        str++;
    }
}
static void printArgInfo(FILE* file, bool isRead, bool isWrite, bool req = false){
    if (isWrite){
        fprintf(file, "ARG_WRITE");
    }
    else if (isRead){
        fprintf(file, "ARG_READ");
    }
    else if(req){
        fprintf(file, "ARG_NOARG");
    }
}

struct InstructionEntry{
    uint8_t code;
    const char* func_name;
    const char* name;
    bool isRead;
    bool isWrite;
};

void printInstrLine(FILE* file, const char* format, InstructionEntry* instr){
    bool opt = false;
    while(*format != '\0'){
        if(opt && instr->name == nullptr){
            if(*format == '%' && *(format+1) == ']'){
                opt = false;
                format++;
            }
            format++;
            continue;
        }

        if(*format == '%'){
            format++;
            switch(*format){
                case '%':
                    fputc('%', file);
                    break;
                case 'c':
                    fprintf(file, "0x%02X", instr->code);
                    break;
                case 'f':
                    printUntilSep(file, instr->func_name);
                    break;
                case 'n':
                    printAlpha(file, instr->name);
                    break;
                case 'a':
                    printArgInfo(file, instr->isRead, instr->isWrite);
                    break;
                case '[':
                    opt = true;
                    break;
                case ']':
                    break;
                default:
                    printf("Unknown instr line format\n");
                    break;
            }
        }
        else{
            fputc(*format, file);
        }
        format++;
    }
}

int main(){

    int cmp_code = system("g++ instr/instructions.h -E -D NOINCLUDE -o " TEMP_FILE_NAME);
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

    for (int i = 0; i <= MAX_INSTRUCTION_CODE; i++){
        instructions[i].code = i;
    }

    Text template_txt = readFileLines("file_templates.txt");
    FILE* out_file = nullptr;
    for(int i = 0; i < template_txt.length; i++){
        char* str = template_txt.lines[i].chars;
        if(*str == '$'){
            str++;
            switch(*str){
                case '$':
                    break;
                case 'F':
                    str++;
                    skipSpaces(str);
                    if(out_file != nullptr){
                        printf("Waring: new file without end of file\n");
                    }
                    out_file = fopen(str, "w");
                    if(out_file == nullptr){
                        printf("File write error\n");
                        return EXIT_FAILURE;
                    }
                    break;
                case '!':
                    fclose(out_file);
                    out_file = nullptr;
                    break;
                case 'L':
                    for (InstructionEntry* i = instructions; i <= instructions + MAX_INSTRUCTION_CODE; i++){
                        if( i->name != nullptr){
                            printInstrLine(out_file, str+1, i);
                            fputc('\n',out_file);
                        }
                    }
                    break;
                case 'A':
                    for (InstructionEntry* i = instructions; i <= instructions + MAX_INSTRUCTION_CODE; i++){
                        printInstrLine(out_file, str+1, i);
                        fputc('\n',out_file);
                    }
                    break;


                default:
                    printf("Unknown command: %c\n", *str);
                    break;

            }
        }
        else{
            if(out_file != nullptr){
                fputs(str, out_file);
                fputc('\n',out_file);
            }
        }
    }
    if(out_file != nullptr){
        printf("Warning: file not closed until end of file\n");
        fclose(out_file);
    }


    free(str.chars);
    nullifyString(&str);
}
