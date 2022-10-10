#pragma once

#include <onegin.h>
#include <stack.h>


enum asm_error_names
{
    ERR_ASM_BAD_FILE     = -9,
    ERR_ASM_BAD_PTR      = -8,
    ERR_ASM_BAD_READ     = -7,
    ERR_ASM_IMP_OPER     = -6,
    ERR_ASM_DIV_ZERO     = -5,
    ERR_ASM_UNKNOWN_OPER = -4,
    ERR_ASM_UNKNOWN_ARG  = -3
};

enum asm_commands
{
    CMD_PUSH = 1,
    CMD_POP  = 2,
    CMD_ADD  = 3,
    CMD_SUB  = 4,
    CMD_MUL  = 5,
    CMD_DIV  = 6,
    CMD_OUT  = 7,
    CMD_HLT  = 8,
    CMD_DUP  = 9,
    CMD_JMP  = 10
};

enum asm_regs
{
    REG_RAX = 1,
    REG_RBX = 2,
    REG_RCX = 3,
    REG_RDX = 4
};

enum asm_args
{
    ARG_IMMED = 16, // 16 = 0x10
    ARG_REG   = 32, // 32 = 0x20
    ARG_RAM   = 64  // 64 = 0x40
};

typedef struct label_t
{
    char* name;
    int value;
}label_t;

typedef struct regs_t
{
    char* name;
    int value;
}regs_t;


#define max_size 100
static const int CP = 0xABADBABE;
static const int count_regs = 4;

int compile(struct string_t* strings, int number_strings, label_t* labelo, int number_labels, int fill_labels);
int count_whitespace(struct string_t str, int count);
int count_labels(struct string_t* strings, int number_strings);
int label_exist(label_t* labels, int number_label, char* cmd);
