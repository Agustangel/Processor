#pragma once

#include <stack.h>


enum error_names
{
    ERR_BAD_ARG       = -10,
    ERR_BAD_FILE      = -9,
    ERR_BAD_PTR       = -8,
    ERR_BAD_READ      = -7,
    ERR_BAD_OPER      = -6,
    ERR_DIV_ZERO      = -5,
    ERR_UNKNOWN_OPER  = -4,
    ERR_UNKNOWN_ARG   = -3,
    ERR_BAD_SIGNATURE = -2
};

enum commands
{
    CMD_PUSH  = 1,
    CMD_POP   = 2,
    CMD_ADD   = 3,
    CMD_SUB   = 4,
    CMD_MUL   = 5,
    CMD_DIV   = 6,
    CMD_OUT   = 7,
    CMD_HLT   = 8,
    CMD_DUP   = 9,
    CMD_JMP   = 10,
    CMD_JB    = 11,
    CMD_JBE   = 12,
    CMD_JA    = 13,
    CMD_JAE   = 14,
    CMD_JE    = 15,
    CMD_JNE   = 16,
    CMD_CALL  = 17,
    CMD_RET   = 18,
    CMD_IN    = 19,
    CMD_SQRT  = 20
};

enum regs
{
    REG_RAX = 1,
    REG_RBX = 2,
    REG_RCX = 3,
    REG_RDX = 4
};

enum args
{
    ARG_IMMED = 32, // 32  = 0x20
    ARG_REG   = 64, // 64  = 0x40
    ARG_RAM   = 128 // 128 = 0x80
};

enum masks
{
    CMD_MASK_1 = 15, // use when code is even
    CMD_MASK_2 = 31  // use when code isn't even
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


#define RAM_SIZE 1000
#define LEN_REG 3
#define LEN_SIGNATURE 4
#define REG_POISON -1
#define max_size 100

static const int CP = 0xABADBABE;
static const int count_regs = 4;

//asm
int compile(struct string_t* strings, int number_strings, label_t* labelo, int* number_labels, int fill_labels);
int count_whitespace(struct string_t str, int count);
int count_labels(struct string_t* strings, int number_strings);
int label_exist(label_t* labels, int number_label, char* cmd);
int fill_label(struct string_t string, label_t* labels, int* count_label, int* position);
int is_label(char* cmd);
void asm_skip_arg(struct string_t string, char* cmd, int count, int* position);
void get_args(struct string_t string, char* code, int* ip, label_t* labels, int number_labels);
void get_ram(struct string_t string, char* code, int* ip, int* n);
void get_immed(int val, char* code, int* ip);
void get_reg(char* str, char* code, int* ip);
void swap_arg(char* code, int* ip);

//cpu
int run(stack_t* stack, char* code, int count, regs_t* Regs, char* RAM);
int check_signature(char* code);
int eval(char* code, int* ip, regs_t* Regs, char* RAM);
void cpu_dump(char* code, int count);
void fill_bits(char n, char* bits);
void convert_binary(char n, char* binary);

//dasm
int decompile(char* code, int count, int* pos_labels, int* real_count_labels, int found_labels);
void dis_eval(FILE* out, char* code, int* ip);
void fprintf_reg(FILE* out, char* code, int* ip);
int dasm_count_labels(char* code, int count);
void skip_arg(char* code, int* ip);
void fill_pos_labels(char* code, int count, int* pos_labels, int* real_count_label);
int dasm_label_exist(int* pos_labels, int count, int positiont);
