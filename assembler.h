#ifndef ASSEMBLER_H_INCLUDED
#define ASSEMBLER_H_INCLUDED

#include "file_read.h"

uint8_t* asmCompile(Text input_txt, size_t* size_ptr);

#endif // ASSEMBLER_H_INCLUDED