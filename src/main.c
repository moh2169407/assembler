#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "diagnostics.h"
#include "svec.h"
#include "main.h"

// Private Functions

static void _print_help_messages() {
    for (int i = 0; i <  HELP_MESSAGE_SIZE; i++) {
        printf("%s", help_message[i]);
    }
}


static int _parse_arugments(int args, char** argv, Mparam* param) {
    // If not arguments are passed
    if (args == 1) {
        Header* header = masm_diagnostics_init_header(0, 0, "masm", true, ERROR_TYPE_FATAL, "no arguments passed");
        Footer* footer = masm_diagnostics_init_footer("try masm --help or -h", FOOTER_TYPE_HELP);

        Message* message = masm_diagnostics_init_message(header, NULL, footer);
        masm_diagnostics_append_message(message);
        return ERROR;
    }
    for (int i = 1; i < args; i++) {
        if (argv[i][0] == '-') {
            if (strcmp("-o", argv[i]) == 0) {
                // -o requires an argument 
                if (args <= (i + 1)) {
                    Header* header = masm_diagnostics_init_header(0, 0, "masm", true, ERROR_TYPE_FATAL, "\"-o\" requires an argument");
                    Footer* footer = masm_diagnostics_init_footer("try masm --help or -h", FOOTER_TYPE_HELP);

                    Message* message = masm_diagnostics_init_message(header, NULL, footer);
                    masm_diagnostics_append_message(message);
                    return ERROR;
                }
                else {
                    param->outFileName = strdup(argv[i + 1]);
                    i++;
                }

            }
            else if (strcmp("-h", argv[i]) == 0 || strcmp("--help", argv[i]) ==0) {
                _print_help_messages();
                // Early out
                return 2;
            }
            else {
                Header* header = masm_diagnostics_init_header(0, 0, "masm", true, ERROR_TYPE_FATAL, "unknown argument \"%s\"", argv[i]);
                Footer* footer = masm_diagnostics_init_footer("try masm --help or -h", FOOTER_TYPE_HELP);

                Message* message = masm_diagnostics_init_message(header, NULL, footer);
                masm_diagnostics_append_message(message);
            }
        }
        else {
            if (param->inFileSize >= param->inFileCapacity) {
                param->inFileCapacity += 3;
                xrealloc(param->inFileNames, sizeof(char*) * param->inFileCapacity);
            }
            param->inFileNames[param->inFileSize++] = argv[i];
        }
    }
    if (param->outFileName == NULL) {
        // Writes "a.out" if no output file is passed
        // overrides if file exists
        param->outFileName = "a.out"; 
    }
    if (param->inFileSize == 0) {
        Header* header = masm_diagnostics_init_header(0, 0, "masm", true, ERROR_TYPE_FATAL, "no input files");
        Footer* footer = masm_diagnostics_init_footer("try masm --help or -h", FOOTER_TYPE_HELP);

        Message* message = masm_diagnostics_init_message(header, NULL, footer);
        masm_diagnostics_append_message(message);
    }
    if (masm_diagnostics_panic()) {
        return ERROR;
    }
    return CLEAN;
}

Mparam _init_mparam(void) {
    return (Mparam) {
        .outFileName = xmalloc(sizeof(char*)),
        .inFileSize = 0,
        .inFileNames = xmalloc(sizeof(char*) * 5),
        .inFileCapacity = 5,
    };  
}

void _free_mparam(Mparam* param) {
    assert(param);
    free(param->inFileNames);
    free(param->outFileName);
}


int main (int args, char** argv) {
    Mparam param = _init_mparam();

    if (_parse_arugments(args, argv, &param)) {
        masm_diagnostics_print_messages();
        masm_diagnostics_free_diagnostic();
        _free_mparam(&param);
        return ERROR;
    }




    
}
