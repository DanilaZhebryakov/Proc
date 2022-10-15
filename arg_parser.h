#ifndef ARG_PARSER_H_INCLUDED
#define ARG_PARSER_H_INCLUDED

#include "lib\logging.h"
#include "instr\instructions_utils.h"

struct label{
    const char* name;
    uint8_t* addr;
};

void initLiteralReplaceArray();

#define skipSpaces(_str) \
    while(isspace(*_str)){ \
        _str++;          \
    }

PROC_DATA_T parseInstrArg(const char* str, uint8_t* out, label** lbl);

#endif // ARG_PARSER_H_INCLUDED
