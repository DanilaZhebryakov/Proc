#include <stdint.h>

typedef int PROC_DATA_T;
#define PROC_DATA_SPEC "%d"

const uint32_t SIGNATURE = 0xDEC0ADDE;
const uint16_t VERSION = 20;

const uint8_t MASK_CMD_CODE = 0x1F;
const uint8_t MASK_CMD_IMM  = 0x20;
const uint8_t MASK_CMD_REG  = 0x40;
const uint8_t MASK_CMD_MEM  = 0x80;

const uint8_t REG_COUNT = 15;

enum instrArgReq_t{
    ARG_NOARG = 0,
    ARG_READ = 1,
    ARG_WRITE = 2
};

bool hasValidReadArg (uint8_t instr);
bool hasValidWriteArg(uint8_t instr);

bool matchesArgReq(uint8_t instr, instrArgReq_t req);


