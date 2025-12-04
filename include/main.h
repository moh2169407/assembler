#ifndef MAIN_H
#define MAIN_H


typedef struct {
    char* outFileName; 
    // TODO 
    // Going to make this a char** and takes a list of inputs files
    char* inFileNames;
    int InFileNums;
} InitParam;

#define HELP_MESSAGE_SIZE 4
const char* help_message [] = {
    "Usage: masm <filename ..>\n",
    "Options:\n",
    "\t-o \t\t\t\tDefines the name of the generated file\n",
    "\t-h \t\t\t\tPrints this help message\n"
};

#endif
