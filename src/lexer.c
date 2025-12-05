#include "lexer.h"
#include <string.h>
#include <stdio.h>

#include "svec.h"


TokenArr* masm_lexer_init_token_arr() {
    TokenArr* tokArr = xmalloc(sizeof(*tokArr));
    tokArr->tokens = xmalloc(sizeof(*tokArr->tokens) * 5);
    tokArr->totalSize = 5;
    tokArr->size = 0;
    return tokArr;
}

void masm_lexer_free_token_arr(TokenArr* tokArr) {
    free(tokArr->tokens);
    free(tokArr);
}


char* masm_lexer_get_new_line(char* buffer, char* delimiter) {
    return strtok(buffer, delimiter);
}

char** masm_lexer_tokenize(char* buffer) {
    // Iterates through the file line by line
    char* current = masm_lexer_get_new_line(buffer, "\n");
    // Cycles through until it reaches the '\0
    while (current != NULL) {
        TokenArr* tok = masm_lexer_slice_line(current);

        // Goes to the next line
        current = masm_lexer_get_new_line(NULL, "\n");
    }
}


TokenArr* masm_lexer_slice_line(char* current) {

}
