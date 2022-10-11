#include <stdio.h>

#include "instructions_lib.h"

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
#define getStackVal(_var)               \
    int _var = 0;                       \
    {                                   \
        stackError_t serr = STACK_NOERROR;  \
        _var = stackPop(stk, &serr);        \
        retStkErr(serr);                    \
    }


#define pushStackVal(_val) {                  \
    stackError_t serr = stackPush(stk, _val); \
    retStkErr(serr);                          \
}


#define getArgVal(_var)             \
    int _var = 0;                   \
    {                               \
        procError_t perr = getInstrArg(*(prog->ip - 1), prog, &_var);\
        if(perr != PROC_NOERROR){   \
            return perr;            \
        }                           \
    }


#define setArgVal(_val) \
    {                   \
        procError_t perr = setInstrArg(*(prog->ip - 1), prog, _val);\
        if(perr != PROC_NOERROR){   \
            return perr; \
        } \
    }


static procError_t instrBad(Processor* prog){
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

    getStackVal(a);
    setArgVal(a);

    return PROC_NOERROR;
}

static procError_t instrPush(Processor* prog){
    Stack* stk = prog->stk;

    getArgVal(a);
    pushStackVal(a);
    return PROC_NOERROR;
}


static procError_t instrSwap(Processor* prog){
    Stack* stk = prog->stk;

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

    getStackVal(b);
    getStackVal(a);
    pushStackVal(a+b);

    return PROC_NOERROR;
}

static procError_t instrSub(Processor* prog){
    Stack* stk = prog->stk;

    getStackVal(b);
    getStackVal(a);
    pushStackVal(a-b);

    return PROC_NOERROR;
}

static procError_t instrMul(Processor* prog){
    Stack* stk = prog->stk;

    getStackVal(b);
    getStackVal(a);
    pushStackVal(a*b);

    return PROC_NOERROR;
}

static procError_t instrDiv(Processor* prog){
    Stack* stk = prog->stk;

    getStackVal(b);
    getStackVal(a);

    if(b == 0)
        return PROC_DIV0;

    pushStackVal(a/b);
    return PROC_NOERROR;
}

static procError_t instrPow(Processor* prog){
    Stack* stk = prog->stk;

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

static procError_t instrJmp(Processor* prog){
    Stack* stk = prog->stk;
    getArgVal(dst);
    prog->ip = prog->prog_data + dst;
    return PROC_NOERROR;
}

#define INSTR_J(_name, _cond) \
static procError_t instrJ##_name (Processor* prog){ \
    Stack* stk = prog->stk;                         \
    getArgVal(dst);                                 \
    getStackVal(b);                                 \
    getStackVal(a);                                 \
    if(a _cond b)                                   \
        prog->ip = prog->prog_data + dst;           \
    return PROC_NOERROR;                            \
}

INSTR_J(gt, > )
INSTR_J(ge, >=)
INSTR_J(lt, < )
INSTR_J(le, <=)
INSTR_J(eq, ==)
INSTR_J(ne, !=)
