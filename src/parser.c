#include "lexer.h"
#include "parser.h"
#include "svec.h"
#include <string.h>

#include "log.h"
#include "lookup.h"

AsmContext* masm_parser_init_context() {
    AsmContext* context = xmalloc(sizeof(*context));
    context->head = NULL; 
    context->tail = NULL;

    return context;
}

void masm_parser_free_context(AsmContext* context) {
    IRNode* node = context->head;
    while (node != NULL) {
        IRNode* next = node->next;
        free(node);
        node = next;
    }
    free(context);
}

void masm_parser_append_node_context(AsmContext* context, IRNode* node) {
    node->next = NULL;
    if (context->head == NULL) {
        // Empty list
        context->head = node;
        context->tail = node;
    } else {
        // Non-empty list
        context->tail->next = node;
        context->tail = node;
    }
}

IRNode* masm_parser_init_instruction_node(const Instruction* instru) {
    IRNode* node = xmalloc(sizeof(*node));
    node->next = NULL;
    node->type = IRNODE_INSTRUCTION;
    node->instance.def = instru;
    node->instance.operandCount = 0;
    // memset(node->instance.operands, 0, sizeof(node->instance.operands));
    return node;
}

IRNode* masm_parser_init_directive_node(Section section) {
    IRNode* node = xmalloc(sizeof(*node));
    node->type = IRNODE_DIRECTIVE;

    node->section = section;
    return node;
}

void masm_parser_free_node(IRNode* head) {
    while (head != NULL) {
        IRNode* next = head->next;
        free(head);
        head = next;
    }
}

AsmContext* masm_parser_analyze(TokenMatrix* matrix)  {
    TokenArr* current = masm_lexer_get_next_token_arr(matrix);    
    AsmContext* context = masm_parser_init_context();
    while (current) {
        masm_parser_lines(current, context); 
        current = masm_lexer_get_next_token_arr(matrix);
    }
    return context;
}


void masm_parser_lines(TokenArr* arr, AsmContext* context) {
    int i = 0;

    while (i < arr->size) {
        Token* tok = &arr->tok[i];

        if (tok->type == TOKEN_TYPE_EOL) {
            i++;
            continue;
        }
        if (tok->type == TOKEN_TYPE_DIRECTIVE) {
            masm_parser_directive(*tok, context);
            i++;
        }
        else if (tok->type == TOKEN_TYPE_LABEL) {
            // i = masm_parser_label(arr, i, context);
            i++;
        }
        else if (tok->type == TOKEN_TYPE_IDENTIF) {
            i = masm_parser_instruction(arr, i, context);
        }
        else {
            log_error("unexpected token [%d:%d] = %s",
                      tok->lineNum, tok->cursorPos,
                      tok->value ? tok->value : "<null>");
            errorCount++;
            i++; // recover
        }
    }
}

void masm_parser_directive(Token token, AsmContext* context) {
    Section section;
    // Is data directive
    if (strcmp(token.value, "data") == 0) {
       section = SECTION_DATA; 
    } 
    else if (strcmp(token.value, "text") == 0) {
        section = SECTION_TEXT;
    }
    else {
        log_error("unknown directive [%d:%d]: %s", token.lineNum, token.cursorPos, token.value);
        errorCount++;
    }
    IRNode* node = masm_parser_init_directive_node(section);
    masm_parser_append_node_context(context, node);
}


int masm_parser_instruction(TokenArr* arr, int i, AsmContext* context) {
    Token* tok = &arr->tok[i];
    const Instruction* ins = masm_lookup_search_instruction(tok->value);

    if (!ins) {
        log_error("unknown instruction [%d:%d]: %s",
                  tok->lineNum, tok->cursorPos, tok->value);
        errorCount++;
        return arr->size;
    }

    IRNode* node = masm_parser_init_instruction_node(ins);
    int operandIndex = 0;

    for (int j = i + 1; j < arr->size && operandIndex < ins->op.operandNum; j++) {
        Token* arg = &arr->tok[j];

        /* Stop at end of line */
        if (arg->type == TOKEN_TYPE_EOL)
            break;

        /* Ignore commas */
        if (arg->type == TOKEN_TYPE_COMMA)
            continue;

        /* Expect register */
        if (arg->type != TOKEN_TYPE_REGISTER) {
            log_error("expected register [%d:%d]",
                      arg->lineNum, arg->cursorPos);
            errorCount++;
            continue;
        }

        const RegisterDef* regDef = masm_lookup_search_register(arg->value);
        if (!regDef) {
            log_error("invalid register [%d:%d]: %s",
                      arg->lineNum, arg->cursorPos, arg->value);
            errorCount++;
            continue;
        }

        node->instance.operands[operandIndex].type = OPERAND_REGISTER;
        node->instance.operands[operandIndex].value = regDef->binary;
        operandIndex++;
    }

    if (operandIndex != ins->op.operandNum) {
        log_error("wrong number of operands for %s (expected %d)",
                  ins->name, ins->op.operandNum);
        errorCount++;
    }

    if (errorCount == 0) {
        node->instance.operandCount = operandIndex;
        masm_parser_append_node_context(context, node);
    }

    return i + 1 + operandIndex * 2; // rough skip (reg + comma)
}
