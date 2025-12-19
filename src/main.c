#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "main.h"
#include "fileio.h"
#include "lexer.h"
#include "svec.h"
#include "log.h"
#include "parser.h"

// TODO
// #include "diagnostic.h"
//
//

typedef struct {
    int binary; // funct code
    int rd;
    int rs;
    int rt;
    int shamt;
} RTypeInstr;

static void masm_main_print_help_message() {
    for (int i = 0; i <  HELP_MESSAGE_SIZE; i++) {
        printf("%s", help_message[i]);
    }
}

static uint32_t encode_rtype(const InstructionInstance* instr) {
    if (instr->operandCount < 3) return 0;

    uint32_t opcode = instr->def->binary & 0xFF;
    uint32_t rs = instr->operands[1].value & 0x1F;
    uint32_t rt = instr->operands[2].value & 0x1F;
    uint32_t rd = instr->operands[0].value & 0x1F;


    return (opcode << 24) | (rs << 16) | (rt << 8) | rd;
}

static int masm_main_parse_arugments(int args, char** argv, InitParam* param) {
    // If not arguments are passed
    if (args == 1) {
        log_error("no arguments passed");
        // Event event = EARLY_ERROR("masm: error: no inputs files\n"); 
        // masm_diagnostic_append_diagnos(event);
        return ERROR;
    }
    // Iterates through the arguments
    for (int i = 1; i < args; i++) {
        if (argv[i][0] == '-') {
            if (strcmp("-T", argv[i]) == 0) {
                param->printTokens = !param->printTokens;
            }
            else if (strcmp("-P", argv[i]) == 0) {
                param->printOutput = !param->printTokens;
            }
            else if (strcmp("-o", argv[i]) == 0) {
                // Checks to see if the next element is within the bounds
                // -o expects an output file name
                // rewrites name if multiple are passed, last argument is saved
                if (args > i + 1) {
                    param->outFileName = argv[i + 1]; 
                    i++;
                }
                else {
                    log_error("\"-o\" takes parameter");
                    // Event event = EARLY_ERROR("masm: error: \"-o\" takes parameters\n");
                    // masm_diagnostic_append_diagnos(event);
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
                log_error("masm: error: unknown argument \"%s\"\n", argv[i]);
                return ERROR;
            }
        } 
        else {
            if (param->inFileNames == NULL) {
                param->inFileNames = argv[i];
                param->InFileNums += 1;
            }
            else {
                log_error("masm: error: passing mulitple input files, \"%s\"\n", argv[i]);
                return ERROR;
            }
        }
    }
    if (param->outFileName == NULL) {
        // Default to a.out as out file name, overwrites if already created
        param->outFileName = "a.out";
    }
    else if (param->inFileNames == NULL) {
        log_error("masm: error: requires inputs files\n");
        return ERROR;
    }
    return CLEAN;
}

void masm_codegen_print_binary(AsmContext* context, InitParam param) {
    FILE* out = stdout;

    if (!param.printOutput) {
        out = fopen(param.outFileName, "wb");
        if (!out) {
            perror("masm: failed to open output file");
            return;
        }
    }
    IRNode* node = context->head;
    while (node) {
        if (node->type == IRNODE_INSTRUCTION) {
            // Human-readable output (debug)
            if (param.printOutput) {
                fprintf(out, "Opcode: %d | Operands:",
                        node->instance.def->binary);

                for (int i = 0; i < node->instance.operandCount; i++) {
                    fprintf(out, " %d", node->instance.operands[i].value);
                }

                uint32_t binaryWord = encode_rtype(&node->instance);
                fprintf(out, " | Encoded: 0x%08X\n", binaryWord);
            }
            else {
                uint32_t binaryWord = encode_rtype(&node->instance);
                fwrite(&binaryWord, sizeof(uint32_t), 1, out);
            }
        }

        node = node->next;
    }

    if (out != stdout) {
        fclose(out);
    }
}

int main (int args, char** argv) {
    InitParam param = {0};
    // Parses the command line arguments
    if (masm_main_parse_arugments(args, argv, &param)) {
        return 1;
    }

    // Reads the file to a char buffer
    char* buffer = masm_fileio_read_file_to_buffer(param.inFileNames);
    // The file couldn't be found 
    if (buffer == NULL) {
        return 1;
    }

    // Parsing 
    TokenMatrix* matrix = masm_lexer_tokenize(buffer);
    if (errorCount > 0) {
        printf("Errors found [lexer]: %d\n", errorCount);
    }

    if (param.printTokens) {
        masm_lexer_matrix_print_formatted_tokens(matrix);
    }

    AsmContext* content =    masm_parser_analyze(matrix);

    if ((!errorCount) > 0) {
        masm_codegen_print_binary(content, param);
    }



    return 0;
}
