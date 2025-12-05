#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "main.h"
#include "fileio.h"
#include "lexer.h"
#include "svec.h"

static void masm_main_print_help_message() {
    for (int i = 0; i <  HELP_MESSAGE_SIZE; i++) {
        printf("%s", help_message[i]);
    }
}

static int masm_main_parse_arugments(int args, char** argv, InitParam* param) {
    // If not arguments are passed
    if (args == 1) {
        fprintf(stderr, "masm: error: no inputs files\n");
        return ERROR;
    }
    // Iterates through the arguments
    for (int i = 1; i < args; i++) {
        // Checks to see if the first char is '-'
        if (argv[i][0] == '-') {
            if (strcmp("-o", argv[i]) == 0) {
                // Checks to see if the next element is within the bounds
                // -o expects an output file name
                // rewrites name if multiple are passed, last argument is saved
                if (args > i + 1) {
                    param->outFileName = argv[i + 1]; 
                    i++;
                }
                else {
                    fprintf(stderr, "masm: error: \"-o\" takes parameters\n");
                    return ERROR;
                }
            }
            // Checks to see if the help arguments
            else if (strcmp("-h", argv[i]) == 0) {
                masm_main_print_help_message();
                // Prints the help message and exits
                return ERROR;
            }
            // Unknown Argument
            else {
                fprintf(stderr, "masm: error: unknown argument \"%s\"\n", argv[i]);
                return ERROR;
            }
        } 
        else {
            if (param->inFileNames == NULL) {
                param->inFileNames = argv[i];
                param->InFileNums += 1;
            }
            else {
                fprintf(stderr, "masm: error: passing mulitple input files, \"%s\"\n", argv[i]);
                return ERROR;
            }
        }
    }
    if (param->outFileName == NULL) {
        // Default to a.out as out file name, overwrites if already created
        param->outFileName = "a.out";
    }
    else if (param->inFileNames == NULL) {
        fprintf(stderr, "masm: error: requires inputs files\n");
        return ERROR;
    }
    return CLEAN;
}

int main (int args, char** argv) {
    InitParam param = {0};
    // Parses the command line arguments
    if (masm_main_parse_arugments(args, argv, &param)) {
        exit(1);
    }

    // Reads the file to a char buffer
    char* buffer = masm_fiolio_read_file_to_buffer(param.inFileNames);
    // The file couldn't be found 
    if (buffer == NULL) {
        exit(1);
    }

    // Parsing 
    masm_lexer_tokenize(buffer);
    return 0;
}
