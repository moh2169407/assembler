#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>

typedef struct {
    char* outFileName; 
    // TODO 
    // Going to make this a char** and takes a list of inputs files
    char* inFileNames;
    int InFileNums;
    bool printTokens;
    bool printOutput;
} InitParam;


#define HELP_MESSAGE_SIZE 4

const char* help_message [] = {
    "Usage: masm <filename ..>\n",
    "Options:\n",
    "\t-o \t\t\t\tDefines the name of the generated file\n",
    "\t-h \t\t\t\tPrints this help message\n"
};

// Error the occur before lexing a file don't have file name, line number or cursor 
// position info
#define EARLY_ERROR(STR) masm_diagnostic_create_event(STR, 0, 0, NULL, ERR_LEVEL_FATAL)

#endif
