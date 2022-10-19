#include <stdio.h>

#include "lib\logging.h"
#include "lib\parseArg.h"
#include "lib\file_read.h"
#include "disassembler.h"

int main(int argc, const char *argv[]){
    initConsole();

    const char* inp_filename = "program.bin";
    int arg_filename = parseArg(argc, argv, "--input");
    if ( (arg_filename != ARG_NOT_FOUND) && (arg_filename + 1 < argc)) {
        inp_filename = argv[arg_filename + 1];
    }

    FILE* out_file = stdout;
    arg_filename = parseArg(argc, argv, "--output");
    if ( (arg_filename != ARG_NOT_FOUND) && (arg_filename + 1 < argc)) {
        out_file = fopen(argv[arg_filename + 1], "r");
        if(out_file == nullptr){
            error_log("Output file not found");
            return 1;
        }
    }



    size_t file_size = 0;
    uint8_t* input_data = (uint8_t*)readBinFile(inp_filename, &file_size);
    if(input_data == nullptr){
        return 1;
    }

    printDisassembly(out_file, input_data, file_size);



}
