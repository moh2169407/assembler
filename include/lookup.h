#ifndef LOOKUP_H
#define LOOKUP_H

#include <stdint.h>

#define INSTRUCTION_MAX_LENGTH 3
#define INSTRUCTION_ARRAY_LENGTH 250
#define REGISTER_TABLE_SIZE 50

typedef enum {
    OPERAND_REGISTER,
    OPERAND_CONSTANT,
    OPERAND_LABEL,
} OperandType;

typedef struct {
    OperandType type;
    union {
        int value;          
        const char* label;  
    };
} Operand;

typedef struct {
    char* name;  
    int binary;
    struct {
        OperandType type[INSTRUCTION_MAX_LENGTH];
        int operandNum;
    } op;
} Instruction;

typedef struct {
    char* name;
    int binary;
} RegisterDef;

typedef struct {
    const Instruction* def;
    Operand operands[INSTRUCTION_MAX_LENGTH]; 
    int operandCount;
} InstructionInstance;

extern Instruction instructionTable[INSTRUCTION_ARRAY_LENGTH];
extern RegisterDef registerTable[REGISTER_TABLE_SIZE];

// Lookup functions
const Instruction* masm_lookup_search_instruction(const char* name);
const RegisterDef* masm_lookup_search_register(const char* name);
InstructionInstance* masm_lookup_init_instruction_instance(const Instruction* instruc);

#endif
