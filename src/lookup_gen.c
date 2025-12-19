#include "lookup.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Instruction instructionTable[INSTRUCTION_ARRAY_LENGTH] = {
    [0] = {
        .name = "ADD",
        .binary = 0,
        .op.type = {OPERAND_REGISTER, OPERAND_REGISTER, OPERAND_REGISTER},
        .op.operandNum = 3,
    },
    [1] = {
        .name = "SUB",
        .binary = 1,
        .op.type = {OPERAND_REGISTER, OPERAND_REGISTER, OPERAND_REGISTER},
        .op.operandNum = 3,
    }
};

RegisterDef registerTable[REGISTER_TABLE_SIZE] = {
    [0] = { .name = "$R0", .binary = 0 },
    [1] = { .name = "$R1", .binary = 1 },
    [2] = { .name = "$R2", .binary = 2 },
    [3] = { .name = "$R3", .binary = 3 }
};

// ---------------- Lookup Functions ----------------

const Instruction* masm_lookup_search_instruction(const char* name) {
    for (int i = 0; i < INSTRUCTION_ARRAY_LENGTH; i++) {
        if (!instructionTable[i].name) break;
        if (strcmp(instructionTable[i].name, name) == 0)
            return &instructionTable[i];
    }
    return NULL;
}

const RegisterDef* masm_lookup_search_register(const char* name) {
    for (int i = 0; i < REGISTER_TABLE_SIZE; i++) {
        if (!registerTable[i].name) break;
        if (strcmp(registerTable[i].name, name) == 0)
            return &registerTable[i];
    }
    return NULL;
}

InstructionInstance* masm_lookup_init_instruction_instance(const Instruction* instruc) {
    InstructionInstance* instance = malloc(sizeof(*instance));
    if (!instance) {
        fprintf(stderr, "Failed to allocate InstructionInstance\n");
        exit(1);
    }
    instance->def = instruc;
    instance->operandCount = 0;
    memset(instance->operands, 0, sizeof(instance->operands));
    return instance;
}
