#pragma once

#include "../onegin/include/onegin.h"
#include "../stack/include/stack.h"


enum asm_error_names
{
    ERR_ASM_BAD_FILE     = -9,
    ERR_ASM_BAD_PTR      = -8,
    ERR_ASM_BAD_READ     = -7,
    ERR_ASM_IMP_OPER     = -6,
    ERR_ASM_DIV_ZERO     = -5,
    ERR_ASM_UNKNOWN_OPER = -4,
};

enum asm_commands
{
    CMD_PUSH = 1,
    CMD_ADD  = 2,
    CMD_SUB  = 3,
    CMD_MUL  = 4,
    CMD_DIV  = 5,
    CMD_OUT  = 6,
    CMD_HLT  = 7
};

static const double epsilon = 0.00001;
const unsigned int CP = 0xABADBABE;

int calculate(stack_t* stack, struct string_t* strings, int number_strings);
int is_zero(double comparison);
int count_whitespace(struct string_t str, int count);
