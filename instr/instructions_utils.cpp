#include "instructions_utils.h"

bool hasValidReadArg(uint8_t instr){
    return instr & (MASK_CMD_REG | MASK_CMD_IMM);
}
bool hasValidWriteArg(uint8_t instr){
    return (instr & MASK_CMD_REG) || (instr & (MASK_CMD_IMM | MASK_CMD_MEM));
}

bool matchesArgReq(uint8_t instr, instrArgReq_t req){
    switch(req){
    case ARG_NOARG:
        return !hasValidReadArg(instr);
    case ARG_READ:
        return hasValidReadArg(instr);
    case ARG_WRITE:
        return hasValidWriteArg(instr);
    default:
        return false;
    }
}
