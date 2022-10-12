#pragma once

#include "assembler.h"


enum cpu_error_names
{
    ERR_CPU_BAD_FILE      = -9,
    ERR_CPU_BAD_PTR       = -8,
    ERR_CPU_BAD_READ      = -7,
    ERR_CPU_IMP_OPER      = -6,
    ERR_CPU_DIV_ZERO      = -5,
    ERR_CPU_UNKNOWN_OPER  = -4,
    ERR_CPU_BAD_SIGNATURE = -3
};

enum asm_args
{
    ARG_IMMED = 16, // 16 = 0x10
    ARG_REG   = 32, // 32 = 0x20
    ARG_RAM   = 64  // 64 = 0x40
};

enum masks
{
    CMD_MASK_1 = 15, // use when code is even
    CMD_MASK_2 = 31  // use when code isn't even
};

int run(stack_t* stack, int* code, int new_count);
int remove_whitespace(int* buffer, long count);
int get_arg(int* code, int* ip);
