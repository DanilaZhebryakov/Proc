#include "debug_utils.h"

void printVarInfo_log(const VarInfo *var){
    setConsoleColor(stderr, COLOR_WHITE, COLOR_BLACK);

    if (var != nullptr){
    fprintf(_logfile, "     Variable info:     Name: %s\n"
                      "                      Status: %s\n"
                      "                  Created at: %s :%d\n"
                      "                 In function: %s\n",
    strPrintable(var->name), varstatusAsString(var->status) , strPrintable(var->file), var->line, strPrintable(var->func));

    fprintf( stderr , "     Variable info:     Name: %s\n"
                      "                      Status: %s\n"
                      "                  Created at: %s :%d\n"
                      "                 In function: %s\n",
    strPrintable(var->name), varstatusAsString(var->status) , strPrintable(var->file), var->line, strPrintable(var->func));
    }
    else{
        fprintf(_logfile, "     Variable info: info is nullptr\n");
        fprintf( stderr , "     Variable info: info is nullptr\n");
    }

    setConsoleColor(stderr, COLOR_DEFAULTT, COLOR_BLACK);

}

const char* varstatusAsString(variableStatus_t var){
    switch(var){
    case VARSTATUS_NEW:
        return "Uninitialised";
    case VARSTATUS_UNUSED:
        return "Initialised";
    case VARSTATUS_NORMAL:
        return "Normal";
    case VARSTATUS_DEAD:
        return "Dead";
    default:
        return "Invalid/uninitialised";
    }
}

const char* strPrintable(const char* str){
    if(str == nullptr){
        return "Nullptr";
    }
    return str;
}

bool checkLCanary(const void* ptr){
    return ((canary_t*)ptr)[-1]     == CANARY_L;
}

bool checkRCanary(const void* ptr, size_t len){
    return ((canary_t*)((char*)ptr+len))[0] == CANARY_R;
}

hash_t gnuHash(const void* begin_ptr, const void* end_ptr){
    hash_t hash = HASH_DEFAULT;
    while(begin_ptr < end_ptr){
        hash = (hash * 33) + (*(uint8_t*)begin_ptr);
        begin_ptr = (uint8_t*)begin_ptr + 1;
    }
    return hash;
}
