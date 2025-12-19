#include "parser.h"
#include "lexer.h"
#include <string.h>

#include "log.h"
#include "svec.h"

AsmContext* masm_parser_init_context() {
    AsmContext* context = xmalloc(sizeof(*context));
    context->section = SECTION_TEXT;
    return context;
}

void masm_parser_free_asm_context(AsmContext* context) {
    free(context);
}





void masm_parser_analyze(TokenMatrix* matrix)  {
    TokenArr* current = masm_lexer_get_next_token_arr(matrix);    
    AsmContext* context = masm_parser_init_context();
    while (current) {
        masm_parser_lines(current, context); 
        current = masm_lexer_get_next_token_arr(matrix);
    }
}


IRNode* masm_paser_create_directive_node(Section section) {
    IRNode* node = xmalloc(sizeof(*node));
    node->type = IRNODE_DIRECTIVE; 
    node->section = section;
    return node;
}

void masm_paser_parse_directive(Token** token, AsmContext* context) {
    Token* t = *token;
    if (strcmp(t->value, "data") == 0) {
        context->section = SECTION_DATA;
        IRNode* node = masm_paser_create_directive_node(SECTION_DATA);
    }
    else if (strcmp(t->value, "text") == 0){
        context->section = SECTION_TEXT;
        IRNode* node = masm_paser_create_directive_node(SECTION_TEXT);
    }
    else {
        log_error("unknown directive: %d:%d\n", t->lineNum, t->cursorPos);
    }
}

void masm_parser_lines(TokenArr* arr, AsmContext* context) {
    Token* token = masm_lexer_get_next_token(arr);
    if (token->type == TOKEN_TYPE_DIRECTIVE) {
        masm_paser_parse_directive(&token, context);
    }
}
