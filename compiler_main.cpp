#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "Console_utils.h"

#include "file_read.h"
#include "parseArg.h"
#include "assembler.h"



int main(int argc, const char *argv[]){
    initConsole();

    const char* inp_filename = "input.txt";
    int arg_filename = parseArg(argc, argv, "--input");
    if ( (arg_filename != ARG_NOT_FOUND) && (arg_filename + 1 < argc)){
        inp_filename = argv[arg_filename + 1];
    }
    Text input_txt = readFileLines(inp_filename);
    if (input_txt.text_data == nullptr){
        return 1;
    }

    const char* out_filename = "program.bin";
    arg_filename = parseArg(argc, argv, "--output");
    if ( (arg_filename != ARG_NOT_FOUND) && (arg_filename + 1 < argc)){
        out_filename = argv[arg_filename + 1];
    }


    size_t program_size = 0;
    uint8_t* output = asmCompile(input_txt, &program_size);
    if (output == nullptr){
        printf("Compilation error\n");
    }

    deleteText(&input_txt);

    for (size_t i = 0; i < program_size; i++){
        printf("%02X ", output[i]);
    }

    FILE* out_file = fopen(out_filename, "wb");
    fwrite(output, sizeof(*output), program_size, out_file);
    fclose(out_file);
    free(output);

    return 0;
}
