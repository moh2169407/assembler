#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>

typedef enum {
    TOKEN_TYPE_INSTR,
    TOKEN_TYPE_REGISTER,
    TOKEN_TYPE_DIRECTIVE,
    TOKEN_TYPE_LABEL,
    TOKEN_TYPE_IDENTIF,
    TOKEN_TYPE_EOL,
    TOKEN_TYPE_NUM_LITERAL,
    TOKEN_TYPE_COMMA,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_UNKNOWN,
    TOKEN_TYPE_DATA_DIRECTIVE,
    TOKEN_TYPE_INVALID,
} TokenType;

typedef struct {
    TokenType type;
    char* value;
    int lineNum;
    int cursorPos;
} Token;

typedef struct {
    Token* tok;
    int size;
    int totalSize;
    int current;
} TokenArr;

typedef struct {
    TokenArr** arr;
    int size;
    int capcity;
    int current;
} TokenMatrix;

typedef struct {
    char* arr;
    int size;
    int totalSize;
} StringBuilder;

// Creates a string builder struct
//
// Used for creating a string by appending one character at a time
// Parameter defines the starting total size of inner array of char
StringBuilder masm_lexer_init_string_builder(int defaultSize);

// Free the internal char* 
//
// Frees the memory allocated for the string, doesn't used if shared pointer
void masm_lexer_free_string_builder(StringBuilder* strB);

// Appends a char to the end of string
void masm_lexer_append_string_builder(StringBuilder* strB, char c);

// Gets the string being built 
//
// Returns the resulting string, appending null term at the very end
char* masm_lexer_to_string_string_builder(StringBuilder* strB);

// Allocates new memory for the string without freeing old string
// Used to pass responsibilities, passing string 
void masm_lexer_reset_string_builder(StringBuilder* strB);

// Takes a buffer that is file loaded into memory and split it 
// returning line by line
char* masm_lexer_get_new_line(char* buffer, char* delimiter);

// Main method that takes a buffer, file written to memory,
// and returns the tokenized version, handles error
TokenMatrix* masm_lexer_tokenize(char* buffer);

// Takes a slices line from buffer and then slices it more
// into tokens 
bool masm_lexer_slice_line(char* current, TokenMatrix* matrix);

// Create a new token
Token masm_lexer_init_token(char* name, TokenType type);

// Adds the column number and line number data to token
void masm_lexer_add_token_metadata(Token* tok, int line, int columnNum);

// Creates a array containing tokens
// need to freed, calling the corresponding free method
TokenArr* masm_lexer_init_token_arr();

// Frees the internal memory used
void masm_lexer_free_token_arr(TokenArr* arr);

// Appends, adding the new token to the end of the array
// TokenArr needs to be initialized
void masm_lexer_append_token_arr(TokenArr* arr, Token* token);

// Create a array containing tokenArr
// needs to be freed
TokenMatrix* masm_lexer_init_token_matrix();

// Free the token matrix, the tokenarr, the strings 
// allocated inside the tokens
void masm_lexer_free_token_matrix(TokenMatrix* matrix);

// Appends a new token array to the ends
// reallocates new memory if needed
void masm_lexer_append_token_matrix(TokenMatrix* matrix, TokenArr* arr);


void masm_lexer_matrix_print_formatted_tokens(TokenMatrix* matrix);

TokenArr* masm_lexer_get_next_token_arr(TokenMatrix* matrix);

Token* masm_lexer_get_next_token(TokenArr* arr);

// Checks to see if the current token is a identifier meaning
// starts with A-Z or a-z or _
// returns false if not true
// or iterates and collect the all the characters that fit this case
bool masm_lexer_is_identifier(char** current, Token* token);

// Checks to see if the current token is a directive meaning
// starts with . and ends with :
// returns false if not true
// or iterates and collect the all the characters that fit this case
//
bool masm_lexer_is_directive(char** current, Token* token);

// Checks to see if the current is at a number
bool masm_lexer_is_number_literal(char** current, Token* token);

bool masm_lexer_is_register(char** current, Token* token);

bool masm_lexer_is_comma(char** current, Token* token);

bool masm_lexer_is_string_literal(char** current, Token* token);

char* _token_type_to_string(TokenType type);


#endif
