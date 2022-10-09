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
};

enum asm_commands
{
    CMD_PUSH = 1,
    CMD_ADD  = 2,
    CMD_SUB  = 3,
    CMD_MUL  = 4,
    CMD_DIV  = 5,
    CMD_OUT  = 6,
    CMD_HLT  = 7,
    CMD_DUP  = 8,
    CMD_JMP  = 9
};

typedef struct label_t
{
    char* name;
    int value;
}label_t;

#define max_size 100
static const unsigned int CP = 0xABADBABE;

int compile(struct string_t* strings, int number_strings, label_t* labelo, int number_labels, int fill_labels);
int count_whitespace(struct string_t str, int count);
int count_labels(struct string_t* strings, int number_strings);
int label_exist(label_t* labels, int number_label, char* cmd);
