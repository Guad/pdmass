//
// Created by Guad on 06-Aug-17.
//

#include "parser.h"
#include "hashtable.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
    ERROR_REGISTER_MISSING,
    ERROR_NUMBER_MISSING,
    ERROR_ADDRESS_MISSING
} error_id;

enum
{
    INSTRUCTION_REG,
    INSTRUCTION_NUM_REG,
    INSTRUCTION_NUM,
    INSTRUCTION_DIR_REG,
    INSTRUCTION_REG_DIR,
    INSTRUCTION_DIR,
    INSTRUCTION_NONE,
};

char* errors[] = {
    "Instruction XXX must have a register name.",
    "Instruction XXX must have a number.",
    "Instruction XXX must have an address."
};

const char *SEP = " ,";

hashtable *instructionId;
hashtable *instructionType;

char* get_error(error_id error, const char inst[4])
{
    errors[error][12] = inst[0];
    errors[error][13] = inst[1];
    if (inst[2]) errors[error][14] = inst[2];

    return errors[error];
}

void error_out(const char *displayError)
{
    if (displayError != NULL)
        puts(displayError);
    exit(1);
}

void set_register(char reg, instruction *inst)
{
    switch (reg)
    {
        case 'B':
            inst->parts.reg = register_B;
            break;
        case 'C':
            inst->parts.reg = register_C;
            break;
        case 'D':
            inst->parts.reg = register_D;
            break;
        case 'E':
            inst->parts.reg = register_E;
            break;
    }
}

void add_mem_addr(char *addr_literal, vector *program)
{
    if (addr_literal[0] == '@')
    {
        // TODO: add labels
    }
    else
    {
        short memaddr = atoi(addr_literal) & 0xFFFF;
        push_item(program, memaddr >> 8);
        push_item(program, memaddr & 0xFF);
    }
}

void init_tables()
{
    instructionId = create_table();
    instructionType = create_table();

    table_set(instructionId, "LD", TO_BYTE(0, 0, 0, 0, 0));
    table_set(instructionType, "LD", INSTRUCTION_REG);

    table_set(instructionId, "ST", TO_BYTE(0, 0, 0, 0, 1));
    table_set(instructionType, "ST", INSTRUCTION_REG);

    table_set(instructionId, "ADD", TO_BYTE(0, 0, 0, 1, 0));
    table_set(instructionType, "ADD", INSTRUCTION_REG);

    table_set(instructionId, "SUB", TO_BYTE(0, 0, 0, 1, 1));
    table_set(instructionType, "SUB", INSTRUCTION_REG);

    table_set(instructionId, "CMP", TO_BYTE(0, 0, 1, 0, 0));
    table_set(instructionType, "CMP", INSTRUCTION_REG);

    table_set(instructionId, "ANA", TO_BYTE(0, 0, 1, 0, 1));
    table_set(instructionType, "ANA", INSTRUCTION_REG);

    table_set(instructionId, "ORA", TO_BYTE(0, 0, 1, 1, 0));
    table_set(instructionType, "ORA", INSTRUCTION_REG);

    table_set(instructionId, "XRA", TO_BYTE(0, 0, 1, 1, 1));
    table_set(instructionType, "XRA", INSTRUCTION_REG);

    table_set(instructionId, "LDI", TO_BYTE(0, 1, 0, 0, 0));
    table_set(instructionType, "LDI", INSTRUCTION_NUM_REG);

    table_set(instructionId, "ADI", TO_BYTE(0, 1, 0, 0, 1));
    table_set(instructionType, "ADI", INSTRUCTION_NUM);

    table_set(instructionId, "SUI", TO_BYTE(0, 1, 0, 1, 0));
    table_set(instructionType, "SUI", INSTRUCTION_NUM);

    table_set(instructionId, "CMI", TO_BYTE(0, 1, 0, 1, 1));
    table_set(instructionType, "CMI", INSTRUCTION_NUM);

    table_set(instructionId, "ANI", TO_BYTE(0, 1, 1, 0, 0));
    table_set(instructionType, "ANI", INSTRUCTION_NUM);

    table_set(instructionId, "ORI", TO_BYTE(0, 1, 1, 0, 1));
    table_set(instructionType, "ORI", INSTRUCTION_NUM);

    table_set(instructionId, "XRI", TO_BYTE(0, 1, 1, 1, 0));
    table_set(instructionType, "XRI", INSTRUCTION_NUM);

    table_set(instructionId, "LDM", TO_BYTE(1, 0, 0, 0, 0));
    table_set(instructionType, "LDM", INSTRUCTION_DIR_REG);

    table_set(instructionId, "STM", TO_BYTE(1, 0, 0, 0, 1));
    table_set(instructionType, "STM", INSTRUCTION_REG_DIR);

    table_set(instructionId, "JMP", TO_BYTE(1, 0, 0, 1, 0));
    table_set(instructionType, "JMP", INSTRUCTION_DIR);

    table_set(instructionId, "BEQ", TO_BYTE(1, 0, 0, 1, 1));
    table_set(instructionType, "BEQ", INSTRUCTION_DIR);

    table_set(instructionId, "BC", TO_BYTE(1, 0, 1, 0, 0));
    table_set(instructionType, "BC", INSTRUCTION_DIR);

    table_set(instructionId, "INC", TO_BYTE(1, 1, 0, 0, 0));
    table_set(instructionType, "INC", INSTRUCTION_NONE);

    table_set(instructionId, "CMA", TO_BYTE(1, 1, 0, 0, 1));
    table_set(instructionType, "CMA", INSTRUCTION_NONE);

    table_set(instructionId, "LF", TO_BYTE(1, 1, 0, 1, 0));
    table_set(instructionType, "LF", INSTRUCTION_NONE);
}

void destroy_tables()
{
    destroy_table(instructionId);
    destroy_table(instructionType);
}

// Returns the number of bytes written to program
int parse_line(char *line, vector *program)
{
    if (!*line) return 0; // no empty lines

    char *instructionName = strtok(line, SEP);

    if (!table_exists(instructionId, instructionName))
    {
        printf("Unknown instruction: %s\n", instructionName);
        error_out(NULL);
    }

    int id = table_get(instructionId, instructionName);
    int type = table_get(instructionType, instructionName);

    instruction inst;
    inst.parts.inst_code = id;

    char *tmpParameter, *tmpParameter2;
    int number, hasNumber = 0;
    char *dirLiteral = NULL;

    switch (type)
    {
        case INSTRUCTION_REG:;
            tmpParameter = strtok(NULL, SEP);

            if (tmpParameter == NULL)
                error_out(get_error(ERROR_REGISTER_MISSING, instructionName));
            set_register(*tmpParameter, &inst);
            break;
        case INSTRUCTION_NUM_REG:
            tmpParameter = strtok(NULL, SEP);

            if (tmpParameter == NULL)
                error_out(get_error(ERROR_NUMBER_MISSING, instructionName));

            tmpParameter2 = strtok(NULL, SEP);

            if (tmpParameter2 == NULL)
                error_out(get_error(ERROR_REGISTER_MISSING, instructionName));

            set_register(*tmpParameter2, &inst);

            number = atoi(tmpParameter);
            hasNumber = 1;
            break;
        case INSTRUCTION_NUM:
            tmpParameter = strtok(NULL, SEP);

            if (tmpParameter == NULL)
                error_out(get_error(ERROR_NUMBER_MISSING, instructionName));

            number = atoi(tmpParameter);
            hasNumber = 1;
            break;
        case INSTRUCTION_DIR_REG:
            tmpParameter = strtok(NULL, SEP);

            if (tmpParameter == NULL)
                error_out(get_error(ERROR_ADDRESS_MISSING, instructionName));

            tmpParameter2 = strtok(NULL, SEP);

            if (tmpParameter2 == NULL)
                error_out(get_error(ERROR_REGISTER_MISSING, instructionName));

            set_register(*tmpParameter2, &inst);

            dirLiteral = tmpParameter;
            break;
        case INSTRUCTION_REG_DIR:
            tmpParameter = strtok(NULL, SEP);

            if (tmpParameter == NULL)
                error_out(get_error(ERROR_REGISTER_MISSING, instructionName));

            tmpParameter2 = strtok(NULL, SEP);

            if (tmpParameter2 == NULL)
                error_out(get_error(ERROR_ADDRESS_MISSING, instructionName));

            set_register(*tmpParameter, &inst);

            dirLiteral = tmpParameter2;
            break;
        case INSTRUCTION_DIR:
            tmpParameter = strtok(NULL, SEP);

            if (tmpParameter == NULL)
                error_out(get_error(ERROR_ADDRESS_MISSING, instructionName));

            dirLiteral = tmpParameter;
            break;
    }

    push_item(program, inst.data);

    if (hasNumber)
        push_item(program, number);

    if (dirLiteral)
        add_mem_addr(dirLiteral, program);
}