#include <stdio.h>
#include <logger.h>
#include <onegin.h>
#include <stack.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include "cpu.h"


//=========================================================================

int run(stack_t* stack, int* code, int new_count, regs_t* Regs, char* RAM)
{
    assert(stack != NULL);
    assert(code != NULL);
    assert(Regs != NULL);    

    int addition = 0;
    int subtraction = 0;
    int multiplication = 0;
    int division = 0;
    int out = 0;
    
    int ret = check_signature(code);
    HANDLE_ERROR(ret, ERR_BAD_SIGNATURE, "ERROR: incorrect signature.\n");

    int ip = count_signature;
    while(ip < new_count)
    {
        switch (code[ip] & CMD_MASK_1)
        {
        case CMD_PUSH:
            stack_push(stack, get_arg(code, &ip, Regs));
            ++ip;
            break;

        case CMD_POP:
            Regs[get_arg(code, &ip, Regs) - 1].value = stack_pop(stack);
            ++ip;
            break;

        case CMD_ADD:
            if(stack->count < 2)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_BAD_OPER);
            }

            addition = stack_pop(stack) + stack_pop(stack);
            stack_push(stack, addition);
            ++ip;
            break;

        case CMD_SUB:
            if(stack->count < 2)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_BAD_OPER);
            }

            subtraction = -(stack_pop(stack) - stack_pop(stack));
            stack_push(stack, subtraction);
            ++ip;
            break;

        case CMD_MUL:
            if(stack->count < 2)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_BAD_OPER);
            }

            multiplication = stack_pop(stack) * stack_pop(stack);
            stack_push(stack, multiplication);
            ++ip;
            break;

        case CMD_DIV:
            if(stack->count < 2)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_BAD_OPER);
            }

            int rhs = stack_pop(stack);
            int lhs = stack_pop(stack);
            if(rhs == 0)
            {
                printf("ERROR: division by zero.\n");
                exit(ERR_DIV_ZERO);
            }

            division = lhs / rhs;
            stack_push(stack, division); 
            ++ip;
            break;

        case CMD_OUT:
            if(stack->count < 1)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_BAD_OPER);  
            }

            out = stack_pop(stack);
            printf("out = %d\n", out);
            ++ip;
            break;

        case CMD_DUP:
            if(stack->count < 1)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_BAD_OPER);  
            }

            out = stack_pop(stack);
            stack_push(stack, out);
            stack_push(stack, out);
            ++ip;
            break;

        case CMD_JMP:
            ip = eval(code, &ip, Regs) + 2; // transition cell number, taking into account the signature
            break;

        case CMD_HLT:
            exit(EXIT_SUCCESS);

        default:
            printf("ERROR: unknown operator.\n");
            exit(ERR_UNKNOWN_OPER);
            break;
        }
    }
    
    return 0;
}

//=========================================================================

int check_signature(int* code)
{
    assert(code != NULL);

    if(code[0] != CP)
    {
        return ERR_BAD_SIGNATURE;
    }
    if(code[1] != version_system)
    {
        return ERR_BAD_VERSION;
    }

    return 0;
}

//=========================================================================

int eval(int* code, int* ip, regs_t* Regs)
{
    assert(code != NULL);
    assert(ip != NULL);

    int cmd = code[*ip];
    int arg = 0;
    ++(*ip);

    if(cmd == CMD_PUSH)
    {
        if(cmd & ARG_IMMED)
        {
            arg += code[*ip];
            if(cmd & ARG_REG)
            {
                ++(*ip);
            }
        }
        if(cmd & ARG_REG)
        {
            arg += Regs[code[*ip] - 1].value;
        }
        if(cmd & ARG_RAM)
        {
            arg = RAM[arg];
        }
    }
    if(cmd == CMD_JMP)
    {
        arg = code[*ip];
    }
    if(cmd == CMD_POP)
    {
        if((cmd & ARG_IMMED) && !(cmd & ARG_RAM))
        {
            LOG("LINE: %d. ERROR: incorrect argument. FUNCTION: %s\n", __LINE__, __PRETTY_FUNCTION__);
            exit(ERR_BAD_ARG);
        }
        if(cmd & ARG_IMMED)
        {
            arg += code[*ip];
            if(cmd & ARG_REG)
            {
                ++(*ip);
            }
        }
        if(cmd & ARG_REG)
        {
            arg += Regs[code[*ip] - 1].value;
        }
        if(cmd & ARG_RAM)
        {
            arg = RAM[arg];
        }
    }

    return arg;
}

//=========================================================================

void cpu_dump(int* code, int new_count)
{
    char binary[8];
    char bits[3];
    int ip = count_signature;

    while(ip < new_count)
    {   
        switch (code[ip] & CMD_MASK_1)
        {
        case CMD_PUSH:
            bits = get_bits(code[ip]);
            binary = convert_binary[code[ip]];
            LOG("%d %x %s (%s #1)\n", code[ip], code[ip], binary, bits);
            ++ip;
            break;

        case CMD_POP:
            bits = get_bits(code[ip]);
            binary = convert_binary[code[ip]];
            LOG("%d %x %s (%s #2)\n", code[ip], code[ip], binary, bits);
            ++ip;
            break;

        case CMD_ADD:
            binary = convert_binary[code[ip]];
            LOG("%d %x %s (#3)\n", code[ip], code[ip], binary);
            ++ip;
            break;

        case CMD_SUB:
            binary = convert_binary[code[ip]];
            LOG("%d %x %s (#4)\n", code[ip], code[ip], binary);
            ++ip;
            break;

        case CMD_MUL:
            binary = convert_binary[code[ip]];
            LOG("%d %x %s (#5)\n", code[ip], code[ip], binary);
            ++ip;
            break;

        case CMD_DIV:
            binary = convert_binary[code[ip]];
            LOG("%d %x %s (#6)\n", code[ip], code[ip], binary);
            ++ip;
            break;

        case CMD_OUT:
            binary = convert_binary[code[ip]];
            LOG("%d %x %s (#7)\n", code[ip], code[ip], binary);
            ++ip;
            break;

        case CMD_HLT:
            binary = convert_binary[code[ip]];
            LOG("%d %x %s (#8)\n", code[ip], code[ip], binary);
            break;

        case CMD_DUP:
            binary = convert_binary[code[ip]];
            LOG("%d %x %s (#9)\n", code[ip], code[ip], binary);
            break;

        case CMD_JMP:
            binary = convert_binary[code[ip]];
            LOG("%d %x %s (#10)\n", code[ip], code[ip], binary);
            break;

        default:
            printf("ERROR: unknown operator.\n");
            exit(ERR_UNKNOWN_OPER);
            break;
        }
    }
}

//=========================================================================

char* get_bits(char n)
{
    char bits[3];
    bits[0] = (n & ARG_RAM)   ? 'M' : 'm';
    bits[1] = (n & ARG_REG)   ? 'R' : 'r';
    bits[2] = (n & ARG_IMMED) ? 'I' : 'i';

    return bits;
}

//=========================================================================

char* convert_binary(char n)
{
    char binary[8];
    for (int i = 0; i < 8; ++i)
    {
        binary[i] = (n & 0x80) ? '1' : '0';
        n <<= 1;
    }

    return binary;
}

//=========================================================================

int remove_whitespace(void* buffer, long count)
{
    assert(buffer != NULL);

    int new_count = 0;
    for(int idx = 0; idx < count; ++idx)
    {
        if(isdigit(buffer[idx]))
        {
            ++new_count;
        }
    }

    return new_count;
}

//=========================================================================

void regs_init(regs_t* Regs)
{
    assert(Regs != NULL);

    for(int idx = 0; idx < count_regs; ++idx)
    {
        Regs[idx].value = REG_POISON;
    }
}

//=========================================================================

int main()
{
    logger_init(1, "cpu.log");
    logger_set_level(INFO);

    stack_t stack;
    int init_size = 10;
    stack_init(&stack, init_size);
    
    FILE* binary_file = fopen("binary.out", "r");
    if (binary_file == NULL)
    {
        printf("ERROR: bad file read.\n");
        exit(ERR_BAD_FILE);
    }

    long count = count_symbols(binary_file);
    HANDLE_ERROR(count, ERR_BAD_PTR, "ERROR: pointer outside file.\n");

    char* buffer = (char*) calloc(count, sizeof(char));
    int ret = fill_buffer(binary_file, buffer, sizeof(char), count);
    HANDLE_ERROR(ret, ERR_BAD_READ, "ERROR: file read error.\n");

    fclose(binary_file);

    regs_t* Regs = (regs_t*) calloc(count_regs, sizeof(regs_t));
    regs_init(Regs);
    
    int new_count = remove_whitespace(buffer, count);
    char* code = (char*) realloc(buffer, new_count * sizeof(char));
    free(buffer);

    char* RAM = (char*) malloc(RAM_SIZE * sizeof(char));

    run(&stack, code, new_count, Regs, RAM);

    stack_dump(&stack);
    stack_destroy(&stack);

    free(Regs);
    free(code);
    free(RAM);

    logger_finalize(file);

    return 0;
}
