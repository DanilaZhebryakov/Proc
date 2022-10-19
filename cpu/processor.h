#ifndef PROCESSOR_H_INCLUDED
#define PROCESSOR_H_INCLUDED

#include "proc_utils.h"

procError_t procRun(Processor* proc);

uint8_t* checkProcSignature(uint8_t* data);

bool procSetProgram(Processor* proc, uint8_t* data, size_t data_size);


#endif // PROCESSOR_H_INCLUDED
