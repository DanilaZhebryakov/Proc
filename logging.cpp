#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <windows.h>

#include "time_utils.h"
#include "Console_utils.h"
#include "debug_utils.h"

#ifndef LOG_FILE_NAME
    #define LOG_FILE_NAME "log.log"
#endif

static const int max_print_args = 100;

FILE* initLogFile();

FILE* _logfile = initLogFile();

void printGoodbyeMsg(){
    fprintf(_logfile, "Program exited\n");
}

FILE* initLogFile(){
    FILE* logfile = fopen(LOG_FILE_NAME, "a");
    if (logfile == nullptr){
        perror("Error opening log file");
    }

    if (setvbuf(logfile, nullptr, _IONBF, 0) != 0){
        perror("Warning: can not set log file buffer");
    }

    fprintf(logfile, "------------------------------------\n");
    fprint_time_date_short(logfile, time(nullptr));
    fprintf(logfile, "Program started\n");
    atexit(printGoodbyeMsg);

    return logfile;
}

void printf_log(const char* format, ...){
    va_list args;
    va_start(args, max_print_args);
    setConsoleColor(stderr, COLOR_WHITE, COLOR_BLACK);
    vfprintf(_logfile, format , args);
    vfprintf( stderr , format , args);
    setConsoleColor(stderr, COLOR_DEFAULTT, COLOR_BLACK);
    va_end(args);
}

void error_log(const char* format, ...){
    va_list args;
    va_start(args, max_print_args);
    setConsoleColor(stderr, (consoleColor)(COLOR_RED | COLOR_INTENSE), COLOR_BLACK);
    fprint_time_nodate(_logfile, time(nullptr));
    fprintf(_logfile, "[ERROR]");
    fprintf( stderr , "[ERROR]");
    vfprintf(_logfile, format , args);
    vfprintf( stderr , format , args);
    setConsoleColor(stderr, COLOR_DEFAULTT, COLOR_BLACK);
    va_end(args);
}

void warn_log(const char* format, ...){
    va_list args;
    va_start(args, max_print_args);
    setConsoleColor(stderr, (consoleColor)(COLOR_YELLOW | COLOR_INTENSE), COLOR_BLACK);
    fprint_time_nodate(_logfile, time(nullptr));
    fprintf(_logfile, "[WARN]");
    fprintf( stderr , "[WARN]");
    vfprintf(_logfile, format , args);
    vfprintf( stderr , format , args);
    setConsoleColor(stderr, COLOR_DEFAULTT, COLOR_BLACK);
    va_end(args);
}
void info_log(const char* format, ...){
    va_list args;
    va_start(args, max_print_args);
    setConsoleColor(stderr, (consoleColor)(COLOR_WHITE), COLOR_BLACK);
    fprint_time_nodate(_logfile, time(nullptr));
    fprintf(_logfile, "[info]");
    fprintf( stderr , "[info]");
    vfprintf(_logfile, format , args);
    vfprintf( stderr , format , args);
    setConsoleColor(stderr, COLOR_DEFAULTT, COLOR_BLACK);
    va_end(args);
}

void debug_log(const char* format, ...){
    va_list args;
    va_start(args, max_print_args);
    setConsoleColor(stderr, COLOR_MAGENTA, COLOR_BLACK);
    fprint_time_nodate(_logfile, time(nullptr));
    fprintf(_logfile, "[DEBUG]");
    fprintf( stderr , "[DEBUG]");
    vfprintf(_logfile, format , args);
    vfprintf( stderr , format , args);
    setConsoleColor(stderr, COLOR_DEFAULTT, COLOR_BLACK);
    va_end(args);
}
void dumpData(const void* begin_ptr, size_t max_size){
    printf_log("     Raw data dump: (%ld total) [ ", max_size);
    size_t i = 0;
    while (i < max_size){
        if (!IsBadReadPtr((char*)begin_ptr + i, 1)){
            printf_log("%p ", ((uint8_t*)begin_ptr)[i]);
        }
        else{
            printf_log("|access denied| ");
        }
        i++;
    }
    printf_log("]\n");
}
