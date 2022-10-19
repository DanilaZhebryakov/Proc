
#ifndef NOINCLUDE
    #include <stdio.h>
    #include <math.h>
#endif

//procError_t (func*)(Stack*     , const char* proc);
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
    PROC_DATA_T _var = 0;                       \
    {                                   \
        stackError_t serr = STACK_NOERROR;  \
        _var = stackPop(proc->stk, &serr);        \
        retStkErr(serr);                    \
    }


#define pushStackVal(_val) {                  \
    stackError_t serr = stackPush(proc->stk, _val); \
    retStkErr(serr);                          \
}


#define getArgVal(_var)             \
    PROC_DATA_T _var = 0;                   \
    {                               \
        procError_t perr = getInstrArg(*(proc->ip - 1), proc, &_var);\
        if(perr != PROC_NOERROR){   \
            return perr;            \
        }                           \
    }


#define setArgVal(_val) \
    {                   \
        procError_t perr = setInstrArg(*(proc->ip - 1), proc, _val);\
        if(perr != PROC_NOERROR){   \
            return perr; \
        } \
    }


static procError_t instrBad(Processor* proc){
    return PROC_BADCMD;
}

static procError_t instrNop(Processor* proc){
    return PROC_NOERROR;
}

static procError_t instrHalt(Processor* proc){
    return PROC_HALT;
}

static procError_t instrDump_0D(Processor* proc){
    programDump(proc);
    return PROC_NOERROR;
}

static procError_t instrPause(Processor* proc){
    sf::Event event;
    while (1)
    {
        if(!((proc->window)->pollEvent(event))){
            sf::sleep(sf::milliseconds(100));
            continue;
        }
        if (event.type == sf::Event::Closed)
            return PROC_USER_STOP;
        if (event.type == sf::Event::KeyPressed){
            return PROC_NOERROR;
        }
    }
    return PROC_NOERROR;
}


static procError_t instrPop(Processor* proc){
    getStackVal(a);
    setArgVal(a);

    return PROC_NOERROR;
}

static procError_t instrPush(Processor* proc){
    getArgVal(a);
    pushStackVal(a);
    return PROC_NOERROR;
}


static procError_t instrSwap(Processor* proc){// can be disabled in this way
    getStackVal(a);
    getStackVal(b);
    pushStackVal(a);
    pushStackVal(b);

    return PROC_NOERROR;
}

static procError_t instrDup(Processor* proc){
    stackError_t serr = STACK_NOERROR;

    PROC_DATA_T a = stackTop(proc->stk, &serr);
    retStkErr(serr);
    pushStackVal(a);

    return PROC_NOERROR;
}

static procError_t instrCount(Processor* proc){
    if(proc->stk == nullptr)
        return PROC_ERRUNK;

    pushStackVal(proc->stk->size);

    return PROC_NOERROR;
}


static procError_t instrGet(Processor* proc){ // can be disabled in this way
    Stack* stk = proc->stk;
    stackError_t serr = STACK_NOERROR;
    PROC_DATA_T a = stackPop(stk, &serr);
    retStkErr(serr);

    PROC_DATA_T val = stackGet(stk, a, &serr);
    retStkErr(serr);
    pushStackVal(val);
    return PROC_NOERROR;
}

#define INSTR_OP(_name, _op ) \
    static procError_t  instr##_name (Processor* proc){ \
        getStackVal(b);         \
        getStackVal(a);         \
        pushStackVal((a) _op (b));      \
        return PROC_NOERROR;    \
    }

INSTR_OP(Add , +  )
INSTR_OP(Sub , -  )
INSTR_OP(Mul , *  )
INSTR_OP(Bsr , << )
INSTR_OP(Bsl , >> )
INSTR_OP(And , &  )
INSTR_OP(Or  , |  )
INSTR_OP(Xor , ^  )

static procError_t instrDiv(Processor* proc){
    getStackVal(b);
    getStackVal(a);

    if(b == 0)
        return PROC_BADMATH;

    pushStackVal(a/b);
    return PROC_NOERROR;
}

static procError_t instrSqrt(Processor* proc){
    getStackVal(a);
    if(a < 0)
        return PROC_BADMATH;

    pushStackVal((PROC_DATA_T)sqrt(a));
    return PROC_NOERROR;
}

static procError_t instrPow(Processor* proc){
    getStackVal(b);
    getStackVal(a);

    PROC_DATA_T res = 1;
    while(b-- > 0)
        res *= a;

    pushStackVal(res);
    return PROC_NOERROR;
}

static procError_t instrInp(Processor* proc){
    PROC_DATA_T val = 0;
    scanf(PROC_DATA_SPEC , &val);
    pushStackVal(val);
    return PROC_NOERROR;
}

static procError_t instrInpCh(Processor* proc){
    PROC_DATA_T val = 0;
    scanf("%c", &val);
    pushStackVal(val);
    return PROC_NOERROR;
}

static procError_t instrOut(Processor* proc){
    stackError_t serr = STACK_NOERROR;

    printf(PROC_DATA_SPEC, stackPop(proc->stk, &serr));
    retStkErr(serr);
    return PROC_NOERROR;
}

static procError_t instrOutCh(Processor* proc){
    stackError_t serr = STACK_NOERROR;

    printf("%c", stackPop(proc->stk, &serr));
    retStkErr(serr);
    return PROC_NOERROR;
}

static procError_t instrJmp(Processor* proc){
    getArgVal(dst);
    proc->ip = proc->prog_data + dst;
    return PROC_NOERROR;
}

static procError_t instrCall(Processor* proc){
    getArgVal(dst);
    pushStackVal(proc->ip - proc->prog_data);
    proc->ip = proc->prog_data + dst;
    return PROC_NOERROR;
}

static procError_t instrRet(Processor* proc){
    getStackVal(dst);
    proc->ip = proc->prog_data + dst;
    return PROC_NOERROR;
}


#define INSTR_J(_name, _cond) \
static procError_t instrJ##_name (Processor* proc){ \
    getArgVal(dst);                                 \
    getStackVal(b);                                 \
    getStackVal(a);                                 \
    if(a _cond b)                                   \
        proc->ip = proc->prog_data + dst;           \
    return PROC_NOERROR;                            \
}

INSTR_J(gt, > )
INSTR_J(ge, >=)
INSTR_J(lt, < )
INSTR_J(le, <=)
INSTR_J(eq, ==)
INSTR_J(ne, !=)
