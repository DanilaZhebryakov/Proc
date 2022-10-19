#ifndef ASSEMBLER_H_INCLUDED
#define ASSEMBLER_H_INCLUDED

#include "lib\file_read.h"

uint8_t* asmCompile(const Text input_txt, size_t* size_ptr, bool stop_on_err = false);

#endif // ASSEMBLER_H_INCLUDED
