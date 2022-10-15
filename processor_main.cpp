#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>



#include "lib\logging.h"
#include "processor.h"
#include "lib\file_read.h"
#include "lib\parseArg.h"



int main(int argc, const char *argv[]) {
    initConsole();


    const char* inp_filename = "program.bin";
    int arg_filename = parseArg(argc, argv, "--input");
    if ( (arg_filename != ARG_NOT_FOUND) && (arg_filename + 1 < argc)) {
        inp_filename = argv[arg_filename + 1];
    }

    size_t file_size = 0;
    uint8_t* input_data = (uint8_t*)readBinFile(inp_filename, &file_size);
    if(input_data == nullptr){
        error_log("File read error");
        return 0;
    }



    sf::RenderWindow window(sf::VideoMode(PROC_WINDOW_HEIGHT * PROC_PX_SIZE, PROC_WINDOW_WIDTH * PROC_PX_SIZE), "Monitor");
    window.clear(sf::Color::Black);
    Processor proc;
    procCtor(&proc);
    proc.window = &window;

    if(! procSetProgram(&proc, input_data, file_size)){
        free(input_data);
        error_log("Bad program signature or version");
        return EXIT_FAILURE;
    }

    procError_t err = procRun(&proc);
    printProcError(err);


    programDump(&proc);
    procDtor(&proc);
    window.close();

    return 0;
}
