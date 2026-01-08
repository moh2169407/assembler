#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>

#define ERROR 1
#define CLEAN 0

typedef struct {
    char* outFileName; 
    char** inFileNames;
    int inFileSize;
    int inFileCapacity;
} Mparam;


#define HELP_MESSAGE_SIZE 4

const char* help_message [] = {
    "Usage: masm <filename ..>\n",
    "Options:\n",
    "\t-o \t\t\t\tDefines the name of the generated file\n",
    "\t-h \t\t\t\tPrints this help message\n"
};

#endif

