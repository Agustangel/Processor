#pragma once


enum error_names
{
    ERR_BAD_FILE      = -9,
    ERR_BAD_PTR       = -8,
    ERR_BAD_READ      = -7,
    ERR_IMP_OPER      = -6,
    ERR_DIV_ZERO      = -5,
    ERR_UNKNOWN_OPER  = -4,
    ERR_UNKNOWN_ARG   = -3,
    ERR_BAD_SIGNATURE = -2,
    ERR_BAD_VERSION   = -1
};

enum commands
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

enum regs
{
    REG_RAX = 1,
    REG_RBX = 2,
    REG_RCX = 3,
    REG_RDX = 4
};

enum args
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


#define LEN_PUSH 4
#define LEN_POP  3
#define LEN_ADD  3
#define LEN_SUB  3
#define LEN_MUL  3
#define LEN_DIV  3
#define LEN_JMP  3
#define LEN_DUP  3
#define LEN_OUT  3
#define LEN_HLT  3
#define LEN_REG  3


#define REG_POISON -1
#define max_size 100
#define count_signature 2

static const int CP = 0xABADBABE;
static int version_system = 0;
static const int count_regs = 4;

static char RAM[1000];

//asm
int compile(struct string_t* strings, int number_strings, label_t* labelo, int number_labels, int fill_labels);
int count_whitespace(struct string_t str, int count);
int count_labels(struct string_t* strings, int number_strings);
int label_exist(label_t* labels, int number_label, char* cmd);
void get_args(struct string_t string, int* code, int* ip, label_t* labels, int number_labels);
void get_immed(int val, int* code, int* ip);
void get_reg(char* str, int* code, int* ip);

//cpu
int run(stack_t* stack, int* code, int new_count, regs_t* Regs);
int check_signature(int* code);
int get_arg(int* code, int* ip, regs_t* Regs);
int remove_whitespace(int* buffer, long count);
int eval(int* code, int* ip, regs_t* Regs);

//dasm
int decompile(int* code, int new_count);
void dis_eval(FILE* out, char* code, int* ip, label_t* labels, int number_labels);
