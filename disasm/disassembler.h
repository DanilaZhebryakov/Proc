#ifndef DISASSEMBLER_H_INCLUDED
#define DISASSEMBLER_H_INCLUDED

#include <stdio.h>

void printInstrArg(FILE* file, uint8_t instr, uint8_t** arg_ptr);


void printDisassembly(FILE* file, uint8_t* input_data, size_t data_size);

#endif // DISASSEMBLER_H_INCLUDED
