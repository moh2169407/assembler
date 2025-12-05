#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_TYPE_INSTR,
    TOKEN_TYPE_REGISTER,
    TOKEN_TYPE_
} TokenType;


typedef struct {
    TokenType type;
    char* value;
} Token;

typedef struct {
    Token* tokens;
    int size;
    int totalSize;
} TokenArr;

TokenArr* masm_lexer_init_token_arr();

void masm_lexer_free_token_arr(TokenArr* tokArr);

char* masm_lexer_get_new_line(char* buffer, char* delimiter);

char** masm_lexer_tokenize(char* buffer);

TokenArr* masm_lexer_slice_line(char* current);



#endif
