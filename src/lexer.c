#include "lexer.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "svec.h"
#include "log.h"

// TODO
// #include "diagnostic.h"

// ---- Private Functions ---------------------------

// TODO
static char* _hex_to_cint(char** current, Token* token);


// TODO
static char* _bin_to_cint(char** current, Token* token);


static bool _is_extra_whitespace_tab(char* current, char* previous) {
    if (*current == ' ' && (previous == NULL || (previous != NULL && *previous == ' ')) || *current == '\t') {
        return true;
    }
    return false;
}

char* _token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_TYPE_INSTR: {
            return "TOKEN_TYPE_IDENTIF";
        }
        case TOKEN_TYPE_REGISTER: {
            return "TOKEN_TYPE_REGISTER";
        }
        case TOKEN_TYPE_DIRECTIVE: {
            return "TOKEN_TYPE_DIRECTIVE";
        }
        case TOKEN_TYPE_LABEL: {
            return "TOKEN_TYPE_LABEL";
        }
        case TOKEN_TYPE_IDENTIF: {
            return "TOKEN_TYPE_IDENTIF";
        }
        case TOKEN_TYPE_EOL: {
            return "TOKEN_TYPE_EOL";
        }
        case TOKEN_TYPE_NUM_LITERAL: {
            return "TOKEN_TYPE_NUM_LITERAL";
        }
        case TOKEN_TYPE_COMMA: {
            return "TOKEN_TYPE_COM";
        }
        case TOKEN_TYPE_STRING: {
            return "TOKEN_TYPE_STRING";
        }
        case TOKEN_TYPE_UNKNOWN: {
            return "TOKEN_TYPE_UNKNOWN";
        }
        case TOKEN_TYPE_DATA_DIRECTIVE:
            return "TOKEN_TYPE_DATA_DIRECTIVE";
        case TOKEN_TYPE_INVALID:
            return "TOKEN_TYPE_INVALID";
        }
}

// ---------------------------------------------------

StringBuilder masm_lexer_init_string_builder(int defaultSize) {
    StringBuilder strB;
    if (defaultSize > 0) {
        strB.arr = xmalloc(defaultSize);
        strB.totalSize = defaultSize;
    }
    else {
        strB.arr = xmalloc(25);
        strB.totalSize = 25;
    }
    strB.size = 0;
    return strB;
}

void masm_lexer_free_string_builder(StringBuilder* strB) {
    free(strB->arr);
}

char* masm_lexer_to_string_string_builder(StringBuilder* strB) {
    if (strB->size >= strB->totalSize) {
        strB->totalSize += 5;
        strB->arr = xrealloc(strB->arr, sizeof(char**) * strB->totalSize);
    }
    strB->arr[strB->size] = '\0';
    return strB->arr;
}


void masm_lexer_append_string_builder(StringBuilder* strB, char c) {
    if (strB->size >= strB->totalSize) {
        strB->totalSize += 5;
        strB->arr = xrealloc(strB->arr, sizeof(char**) * strB->totalSize);
    }
    strB->arr[strB->size++] = c;
}

void masm_lexer_reset_string_builder(StringBuilder* strB) {
    strB->arr = xmalloc(10);
    strB->totalSize = 10;
    strB->size = 0;
}

char* masm_lexer_get_new_line(char* buffer, char* delimiter) {
    return strtok(buffer, delimiter);
}

TokenMatrix* masm_lexer_tokenize(char* buffer) {
    // Iterates through the file line by line
    char* current = masm_lexer_get_new_line(buffer, "\n");
    TokenMatrix* matrix = masm_lexer_init_token_matrix();
    // Cycles through until it reaches the '\0
    while (current != NULL) {

        masm_lexer_slice_line(current, matrix);

        // Goes to the next line
        current = masm_lexer_get_new_line(NULL, "\n");
    }

    // return matrix;
    return matrix;
}

bool masm_lexer_slice_line(char* line, TokenMatrix* matrix) {
    // Copies the pointer at the beginning of the line
    char* current = line;
    // Tracks previous current value
    char *previous = line;
    // Tracks the cursor position 
    char* pos = current;

    TokenArr* arr = masm_lexer_init_token_arr();

    // Iterates through the entire line until null term is reached
    while (*current) {
        // checks to see if there's consecutively whitespaces
        // and tabs to skips over
        if (_is_extra_whitespace_tab(current, previous)) {
            previous = current;
            current++;
            pos++;
            continue;
        }
        // ';' are comments
        if (*current == ';') {
            break;
        }

        Token tok;
        tok.lineNum = matrix->size + 1;
        // TODO Use an array of function pointers
        //
        if (!masm_lexer_is_directive(&current, &tok) &&
            !masm_lexer_is_identifier(&current, &tok) && 
            !masm_lexer_is_register(&current, &tok) && 
            !masm_lexer_is_number_literal(&current, &tok) &&
            !masm_lexer_is_comma(&current, &tok) &&
            !masm_lexer_is_string_literal(&current, &tok)) 
        {
            log_error("unknown char: %c\n", *current);
            errorCount++;

            // Turning the char in null term string
            char buf[2] = { *current, '\0' };

            tok = masm_lexer_init_token(strdup(buf), TOKEN_TYPE_UNKNOWN);

            current++;
        }
        // Calculates the cursor position
        int columnNum = pos - line; 
        masm_lexer_add_token_metadata(&tok, matrix->size + 1, columnNum + 1);

        masm_lexer_append_token_arr(arr, &tok);
        previous = current;
        pos = current;
    }
    // Appends the End of Line token to the array
    Token tok = masm_lexer_init_token(NULL, TOKEN_TYPE_EOL);
    masm_lexer_add_token_metadata(&tok, matrix->size + 1, current - line);
    masm_lexer_append_token_arr(arr, &tok);

    masm_lexer_append_token_matrix(matrix, arr);

    return true;
}

Token masm_lexer_init_token(char* name, TokenType type) {
    return (Token) {
        .value = name,
        .type = type,
    };
}

void masm_lexer_add_token_metadata(Token* tok, int line, int columnNum) {
    tok->lineNum = line;
    tok->cursorPos = columnNum;
}

TokenArr* masm_lexer_init_token_arr() {
    TokenArr* tokenArr = xmalloc(sizeof(*tokenArr));
    tokenArr->totalSize = 5;
    tokenArr->tok = xmalloc(sizeof(*tokenArr->tok) * tokenArr->totalSize);
    return tokenArr;
}

void masm_lexer_free_token_arr(TokenArr* arr) {
    free(arr->tok);
    free(arr);
}

void masm_lexer_append_token_arr(TokenArr* arr, Token* token) {
    if (arr->size >= arr->totalSize) {
        arr->totalSize += 5;
        arr->tok = xrealloc(arr->tok, sizeof(*arr->tok) * arr->totalSize);
    }
    arr->tok[arr->size++] = *token;
}

TokenMatrix* masm_lexer_init_token_matrix() {
    TokenMatrix* matrix = xmalloc(sizeof(*matrix));
    matrix->capcity = 5;
    matrix->current = 0;
    matrix->arr = xmalloc(sizeof(*matrix->arr) * matrix->capcity);
    matrix->size = 0;
    return matrix;
}

void masm_lexer_free_token_matrix(TokenMatrix* matrix) {
    for (int i = 0; i < matrix->size; i++) {
        masm_lexer_free_token_arr(matrix->arr[i]);
    }
    free(matrix);
}

void masm_lexer_append_token_matrix(TokenMatrix* matrix, TokenArr* arr) {
    if (matrix->size >= matrix->capcity) {
        matrix->capcity += 5;
        matrix->arr = xrealloc(matrix->arr, sizeof(*matrix->arr) * matrix->capcity);
    }
    matrix->arr[matrix->size++] = arr;
}

void masm_lexer_matrix_print_formatted_tokens(TokenMatrix* matrix) {
    for (int i = 0; i < matrix->size; i++) {
        for (int j = 0; j < matrix->arr[i]->size; j++) {
            Token tok = matrix->arr[i]->tok[j];
            if (tok.type == TOKEN_TYPE_EOL) {
                printf("[%d:%d]  %s\n", tok.lineNum, tok.cursorPos, _token_type_to_string(tok.type));
            }
            else {
                printf("[%d:%d]  %s : %s\n", tok.lineNum, tok.cursorPos, _token_type_to_string(tok.type), tok.value);
            }
        }
    } 
}

bool masm_lexer_is_identifier(char** current, Token* token) {
    char* p = *current;
    TokenType type;
    // identifiers starts with characters or '_'
    if (!isalpha(*p) && *p != '_') {
        return false;
    }
    
    // Creates a string builder with a default size of 10
    StringBuilder strB = masm_lexer_init_string_builder(10);
    type = TOKEN_TYPE_IDENTIF;
    while (*p) {
        if (*p == ':') {
            type = TOKEN_TYPE_LABEL;
            p++;
            break; 
        }
        if (!isalpha(*p) && *p != '_') {
            break;
        }
        masm_lexer_append_string_builder(&strB, *p);
        p++;
    }

    char* name = masm_lexer_to_string_string_builder(&strB);
    *token = masm_lexer_init_token(name, type);

    *current = p;
    return true;
}

bool masm_lexer_is_directive(char** current, Token* token) {
    char* p = *current;
    if (*p != '.') {
        return false;
    }  
    p++;

    // Creates a string builder with a default size of 10
    StringBuilder strB = masm_lexer_init_string_builder(10);
    TokenType type;

    while (*p) {
        if (*p == ':') {
            type = TOKEN_TYPE_DIRECTIVE;
            p++;
            break; 
        }
        if (!isalpha(*p)) {
            type = TOKEN_TYPE_DATA_DIRECTIVE;
            break;            
        }
        masm_lexer_append_string_builder(&strB, *p);
        p++;
    }

    char* name = masm_lexer_to_string_string_builder(&strB);
    *token = masm_lexer_init_token(name, type);

    *current = p;
    return true;
}

bool masm_lexer_is_register(char** current, Token* token) {
    char* p = *current;
    // Register starting with '$'
    if (*p != '$') {
        return false; 
    }
    // Creates a string builder with a default size of 10
    StringBuilder strB = masm_lexer_init_string_builder(10);
    masm_lexer_append_string_builder(&strB, *p);
    p++;

    while (*p) {
        if (!isalpha(*p) && !isdigit(*p)) {
            break;
        }

        masm_lexer_append_string_builder(&strB, *p);
        p++;
    }
    
    char* name = masm_lexer_to_string_string_builder(&strB);
    *token = masm_lexer_init_token(name, TOKEN_TYPE_REGISTER);

    *current = p;
    return true;
}

bool masm_lexer_is_number_literal(char** current, Token *token) {
    char* p = *current;
    char* next = p + 1;

    if (!isdigit(*p)) {
       return false; 
    }


    // TODO add support for hex and binary numbers
   
    
    // // Handles Hex literals
    // if (next != NULL && *next == 'x') {
    //     _hex_to_cint(current, token);
    // }
    //
    // // Handles binary literals
    // else if (next != NULL && *next == 'b') {
    //     _bin_to_cint(current, token);
    // }
    // // Regular Literals
    // else {
    //
    // }


    // Creates a string builder with a default size of 10
    StringBuilder strB = masm_lexer_init_string_builder(10);

    while (*p) {
        if (!isdigit(*p)) {
            break;
        }
        masm_lexer_append_string_builder(&strB, *p);
        p++;
    }

    char* name = masm_lexer_to_string_string_builder(&strB);
    *token = masm_lexer_init_token(name, TOKEN_TYPE_NUM_LITERAL);

    *current = p;
    return true;

}

TokenArr* masm_lexer_get_next_token_arr(TokenMatrix* matrix) {
    if (matrix->current <= matrix->size) {
        return matrix->arr[matrix->current++];
    }
    return NULL;
}

Token* masm_lexer_get_next_token(TokenArr* arr) {
    if (arr->current <= arr->size) {
        return &arr->tok[arr->current++];
    }
    return NULL;
}

bool masm_lexer_is_comma(char** current, Token* token) {
    char* p = *current;
    if (*p != ',') {
        return false;
    }
    else {
        *token = masm_lexer_init_token(",", TOKEN_TYPE_COMMA);
        *current = ++p;
        return true;
    }
}

bool masm_lexer_is_string_literal(char** current, Token* token) {
    char* p = *current;

    if (*p != '\"') {
        return false; 
    }
    // Creates a string builder with a default size of 10
    StringBuilder strB = masm_lexer_init_string_builder(10);

    // Moves pass current \"
    p = p + 1;
    bool closed = false;

    while (*p) {
        if (*p == '\"') {
            closed = true;
            break;
        }
        masm_lexer_append_string_builder(&strB, *p);
        p++;
    }
    
    char* name = masm_lexer_to_string_string_builder(&strB);
    *token = masm_lexer_init_token(name, closed ? TOKEN_TYPE_STRING : TOKEN_TYPE_INVALID);
    if (closed) {
        p++;
        *current = p;
    }
    else {
        errorCount++;
        *current = p;
        log_error("%s\n", "open string\n");
    }
    return true;
}
