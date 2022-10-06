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



static procError_t instrNotExists(Stack* stk, Program* prog){
    return PROC_BADCMD;
}

static procError_t instrNop(Stack* stk, Program* prog){
    return PROC_NOERROR;
}

static procError_t instrHalt(Stack* stk, Program* prog){
    return PROC_HALT;
}

static procError_t instrDump(Stack* stk, Program* prog){
    programDump(prog);
    stackDump(stk);
    return PROC_NOERROR;
}



static procError_t instrPop(Stack* stk, Program* prog){
    stackError_t serr = STACK_NOERROR;
    stackPop(stk, &serr);
    retStkErr(serr);
    return PROC_NOERROR;
}

static procError_t instrPush(Stack* stk, Program* prog){
    stackError_t serr = stackPush(stk, getInstrArg(prog));
    retStkErr(serr);
    return PROC_NOERROR;
}


static procError_t instrSwap(Stack* stk, Program* prog){
    stackError_t serr = STACK_NOERROR;
    int b = stackPop(stk, &serr);
    retStkErr(serr);
    int a = stackPop(stk, &serr);
    retStkErr(serr);
    serr = stackPush(stk, b);
    retStkErr(serr);
    serr = stackPush(stk, a);
    retStkErr(serr);
    return PROC_NOERROR;
}

static procError_t instrDup(Stack* stk, Program* prog){
    stackError_t serr = STACK_NOERROR;
    int a = stackTop(stk, &serr);
    retStkErr(serr);

    serr = stackPush(stk, a);
    retStkErr(serr);
    return PROC_NOERROR;
}

static procError_t instrCount(Stack* stk, Program* prog){
    if(stk == nullptr)
        return PROC_ERRUNK;

    stackError_t serr = stackPush(stk, stk->size);
    retStkErr(serr);
    return PROC_NOERROR;
}

static procError_t instrGet(Stack* stk, Program* prog){
    stackError_t serr = STACK_NOERROR;
    int a = stackPop(stk, &serr);
    retStkErr(serr);

    int val = stackGet(stk, a, &serr);
    retStkErr(serr);
    serr = stackPush(stk, val);
    retStkErr(serr);
    return PROC_NOERROR;
}



static procError_t instrAdd(Stack* stk, Program* prog){
    stackError_t serr = STACK_NOERROR;
    int b = stackPop(stk, &serr);
    retStkErr(serr);
    int a = stackPop(stk, &serr);
    retStkErr(serr);
    serr = stackPush(stk, a+b);
    retStkErr(serr);
    return PROC_NOERROR;
}

static procError_t instrSub(Stack* stk, Program* prog){
    stackError_t serr = STACK_NOERROR;
    int b = stackPop(stk, &serr);
    retStkErr(serr);
    int a = stackPop(stk, &serr);
    retStkErr(serr);
    serr = stackPush(stk, a-b);
    retStkErr(serr);
    return PROC_NOERROR;
}

static procError_t instrMul(Stack* stk, Program* prog){
    stackError_t serr = STACK_NOERROR;
    int b = stackPop(stk, &serr);
    retStkErr(serr);
    int a = stackPop(stk, &serr);
    retStkErr(serr);
    serr = stackPush(stk, a*b);
    retStkErr(serr);
    return PROC_NOERROR;
}

static procError_t instrDiv(Stack* stk, Program* prog){
    stackError_t serr = STACK_NOERROR;
    int b = stackPop(stk, &serr);
    retStkErr(serr);
    int a = stackPop(stk, &serr);
    retStkErr(serr);
    if(b == 0){
        return PROC_DIV0;
    }
    serr = stackPush(stk, a/b);
    retStkErr(serr);
    return PROC_NOERROR;
}

static procError_t instrPow(Stack* stk, Program* prog){
    stackError_t serr = STACK_NOERROR;
    int b = stackPop(stk, &serr);
    retStkErr(serr);
    int a = stackPop(stk, &serr);
    retStkErr(serr);

    int res = 1;
    while(b-- > 0)
        res *= a;

    serr = stackPush(stk, res);
    retStkErr(serr);
    return PROC_NOERROR;
}

static procError_t instrInp(Stack* stk, Program* prog){
    int val = 0;
    scanf("%d", &val);
    stackError_t serr = stackPush(stk, val);
    retStkErr(serr);
    return PROC_NOERROR;
}

static procError_t instrInpCh(Stack* stk, Program* prog){
    int val = 0;
    scanf("%c", &val);
    stackError_t serr = stackPush(stk, val);
    retStkErr(serr);
    return PROC_NOERROR;
}

static procError_t instrOut(Stack* stk, Program* prog){
    stackError_t serr = STACK_NOERROR;
    printf("%d\n", stackPop(stk, &serr));
    retStkErr(serr);
    return PROC_NOERROR;
}

static procError_t instrOutCh(Stack* stk, Program* prog){
    stackError_t serr = STACK_NOERROR;
    printf("%d\n", stackPop(stk, &serr));
    retStkErr(serr);
    return PROC_NOERROR;
}


const struct Instruction INSTR_LIST[] = {
   {0x00, instrNotExists, "bad " },

// {code, func      , "name" }
   {0x01, instrNop  , "nop"  },
   {0x02, instrHalt , "halt" },
   {0x0D, instrDump , "dump" },

   {0x10, instrPop  , "pop"  },
   {0x11, instrPush , "push" },
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
