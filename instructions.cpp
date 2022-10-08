#include <stdio.h>

#include "instructions.h"

//procError_t (func*)(Stack*     , const char* prog);
#define retStkErr(_serr)                                \
    if (_serr != 0){                                     \
        int perr = PROC_NOERROR;                \
        if (_serr & STACK_OP_INVALID)                     \
            perr |= PROC_STACKEMPT;                     \
        if (_serr & STACK_OP_ERROR)                       \
            perr |= PROC_INT_ERROR;                     \
        if (_serr & ~(STACK_OP_ERROR | STACK_OP_INVALID)) \
            perr |= PROC_ERRUNK;                        \
        return (procError_t)perr;                                    \
    }

    //! pressanykey 0x03
    //! dump 0x0D +-
#define getStackVal(_var)  \
    int _var = stackPop(stk, &serr); \
    retStkErr(serr); \


#define pushStackVal(_val) { \
    stackError_t serr = stackPush(stk, _val); \
    retStkErr(serr); \
}


static procError_t instrNotExists(Processor* prog){
    return PROC_BADCMD;
}

static procError_t instrNop(Processor* prog){
    return PROC_NOERROR;
}

static procError_t instrHalt(Processor* prog){
    return PROC_HALT;
}

static procError_t instrDump(Processor* prog){
    programDump(prog);
    return PROC_NOERROR;
}



static procError_t instrPop(Processor* prog){
    Stack* stk = prog->stk;
    stackError_t serr = STACK_NOERROR;

    getStackVal(a);
    return PROC_NOERROR;
}

static procError_t instrPush(Processor* prog){
    Stack* stk = prog->stk;
    pushStackVal(getInstrArg(prog));
    return PROC_NOERROR;
}


static procError_t instrSwap(Processor* prog){
    Stack* stk = prog->stk;
    stackError_t serr = STACK_NOERROR;
    getStackVal(a);
    getStackVal(b);
    pushStackVal(a);
    pushStackVal(b);
    return PROC_NOERROR;
}

static procError_t instrDup(Processor* prog){
    Stack* stk = prog->stk;
    stackError_t serr = STACK_NOERROR;

    int a = stackTop(stk, &serr);
    retStkErr(serr);

    pushStackVal(a);
    return PROC_NOERROR;
}

static procError_t instrCount(Processor* prog){
    Stack* stk = prog->stk;
    if(stk == nullptr)
        return PROC_ERRUNK;

    pushStackVal(stk->size);
    return PROC_NOERROR;
}

static procError_t instrGet(Processor* prog){
    Stack* stk = prog->stk;
    stackError_t serr = STACK_NOERROR;
    int a = stackPop(stk, &serr);
    retStkErr(serr);

    int val = stackGet(stk, a, &serr);
    retStkErr(serr);
    pushStackVal(val);
    return PROC_NOERROR;
}



static procError_t instrAdd(Processor* prog){
    Stack* stk = prog->stk;
    stackError_t serr = STACK_NOERROR;

    getStackVal(b);
    getStackVal(a);

    pushStackVal(a+b);
    return PROC_NOERROR;
}

static procError_t instrSub(Processor* prog){
    Stack* stk = prog->stk;
    stackError_t serr = STACK_NOERROR;

    getStackVal(b);
    getStackVal(a);

    pushStackVal(a-b);
    return PROC_NOERROR;
}

static procError_t instrMul(Processor* prog){
    Stack* stk = prog->stk;
    stackError_t serr = STACK_NOERROR;

    getStackVal(b);
    getStackVal(a);

    pushStackVal(a*b);
    return PROC_NOERROR;
}

static procError_t instrDiv(Processor* prog){
    Stack* stk = prog->stk;
    stackError_t serr = STACK_NOERROR;

    getStackVal(b);
    getStackVal(a);

    retStkErr(serr);
    if(b == 0)
        return PROC_DIV0;

    pushStackVal(a/b);
    return PROC_NOERROR;
}

static procError_t instrPow(Processor* prog){
    Stack* stk = prog->stk;
    stackError_t serr = STACK_NOERROR;

    getStackVal(b);
    getStackVal(a);

    int res = 1;
    while(b-- > 0)
        res *= a;

    pushStackVal(res);
    return PROC_NOERROR;
}

static procError_t instrInp(Processor* prog){
    Stack* stk = prog->stk;
    int val = 0;
    scanf("%d", &val);
    pushStackVal(val);
    return PROC_NOERROR;
}

static procError_t instrInpCh(Processor* prog){
    Stack* stk = prog->stk;
    int val = 0;
    scanf("%c", &val);
    pushStackVal(val);
    return PROC_NOERROR;
}

static procError_t instrOut(Processor* prog){
    Stack* stk = prog->stk;
    stackError_t serr = STACK_NOERROR;

    printf("%d\n", stackPop(stk, &serr));
    retStkErr(serr);
    return PROC_NOERROR;
}

static procError_t instrOutCh(Processor* prog){
    Stack* stk = prog->stk;
    stackError_t serr = STACK_NOERROR;

    printf("%d\n", stackPop(stk, &serr));
    retStkErr(serr);
    return PROC_NOERROR;
}


const struct Instruction INSTR_LIST[] = {
   {0x00, instrNotExists, "bad " },

// {code, func      , "name" , arg?}
   {0x01, instrNop  , "nop"  },
   {0x02, instrHalt , "halt" },
   {0x0D, instrDump , "dump" },

   {0x10, instrPop  , "pop"  },
   {0x11, instrPush , "push" , 1},
   {0x12, instrDup  , "dup"  },
   {0x13, instrSwap , "swap" },
   {0x14, instrCount, "count"},
   {0x15, instrGet  , "get"  },

   {0x21, instrAdd  , "add"  },
   {0x22, instrSub  , "sub"  },
   {0x23, instrMul  , "mul"  },
   {0x24, instrDiv  , "div"  },
   {0x25, instrPow  , "pow"  },

   {0x30, instrInp  , "inp"  },
   {0x31, instrOut  , "out"  },
   {0x32, instrInpCh, "inpch"},
   {0x33, instrOutCh, "outch"},

   {0xFF, instrNotExists, ""}
};

const int INSTR_COUNT = sizeof(INSTR_LIST) / sizeof(Instruction);
