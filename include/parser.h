#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "lookup.h"

// typedef enum {
//     LABEL_TYPE_DATA,
//     LABEL_TYPE_POS,
// } LabelType;

typedef enum {
    IRNODE_INSTRUCTION,
    IRNODE_LABEL,
    IRNODE_DIRECTIVE
} IRNodeType;

// typedef enum {
//     DATA_LABEL_ASCII,
//     DATA_LABEL_ASCIIZ,
//     DATA_LABEL_WORD,
//     DATA_LABEL_HALF,
//     DATA_LABEL_HHALF,
//     DATA_LABEL_BYTE,
// } DataLabelType;

// typedef struct {
//     char* name;
//     LabelType type;
//     union {
//         int address;
//         struct {
//             DataLabelType dataType; 
//             void* p;
//         } data;
//     };
// } Label;
//
// typedef struct {
//     Label* arr;
//     int size;
//     int capacity;
// } LabelArr;

typedef enum {
    SECTION_DATA,
    SECTION_TEXT,
} Section;


typedef struct IRNode {
    struct IRNode* next;
    IRNodeType type;

    union {
        // Label label;
        //
        Section section;
        InstructionInstance instance;
    };
} IRNode;

typedef struct {
    int instructionLen;
    int dataLen; 
    Section section;

    IRNode* head;
    IRNode* tail;

} AsmContext;


AsmContext* masm_parser_init_context();

void masm_parser_free_context(AsmContext* context);

void masm_parser_append_node_context(AsmContext* context, IRNode* node);


IRNode* masm_parser_init_instruction_node(const Instruction* instru);


IRNode* masm_parser_init_directive_node(Section section);


void masm_parser_free_node(IRNode* head);


AsmContext* masm_parser_analyze(TokenMatrix* matrix) ;


void masm_parser_lines(TokenArr* arr, AsmContext* context);

void masm_parser_directive(Token token, AsmContext* context);

int masm_parser_instruction(TokenArr* arr, int i, AsmContext* context);


// LabelArr* masm_parser_init_label_arr();
//
// void masm_parser_free_label_arr(LabelArr* LabelArr);
//
// void masm_parser_append_label_arr(LabelArr* laberArr);
//
// Label masm_parser_create_data_label();
//
// Label masm_paser_create_pos_label();
//
// bool masm_paser_search_label(char* name);
//
//
// void masm_parser_analyze(TokenMatrix* matrix);
//
// AsmContext* masm_parser_init_context();
//
// void masm_parser_lines(TokenArr* arr, AsmContext* context);
//
// void masm_parser_identifiers();
//
// AsmContext* masm_parser_init_context();
//
//

#endif
