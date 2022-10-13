#ifndef INSTRUCTIONS_COMPILE_H_INCLUDED
#define INSTRUCTIONS_COMPILE_H_INCLUDED
    #include "instructions_utils.h"
    struct CompileInstruction{
        uint8_t code;
        const char* name;
        instrArgReq_t arg_req;
    };

    extern const struct CompileInstruction CMP_INSTR_LIST[];
    extern const int CMP_INSTR_COUNT;


#endif // INSTRUCTIONS_COMPILE_H_INCLUDED
