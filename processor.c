#include <stdio.h>
#include <logger.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include "processor.h"


//=========================================================================

int run(stack_t* stack, int* code, int new_count, regs_t* Regs)
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
    HANDLE_ERROR(ret, ERR_CPU_BAD_SIGNATURE, "ERROR: incorrect signature.\n");

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
                exit(ERR_CPU_IMP_OPER);
            }

            addition = stack_pop(stack) + stack_pop(stack);
            stack_push(stack, addition);
            ++ip;
            break;

        case CMD_SUB:
            if(stack->count < 2)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_CPU_IMP_OPER);
            }

            subtraction = -(stack_pop(stack) - stack_pop(stack));
            stack_push(stack, subtraction);
            ++ip;
            break;

        case CMD_MUL:
            if(stack->count < 2)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_CPU_IMP_OPER);
            }

            multiplication = stack_pop(stack) * stack_pop(stack);
            stack_push(stack, multiplication);
            ++ip;
            break;

        case CMD_DIV:
            if(stack->count < 2)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_CPU_IMP_OPER);
            }

            int rhs = stack_pop(stack);
            int lhs = stack_pop(stack);
            if(rhs == 0)
            {
                printf("ERROR: division by zero.\n");
                exit(ERR_CPU_DIV_ZERO);
            }

            division = lhs / rhs;
            stack_push(stack, division); 
            ++ip;
            break;

        case CMD_OUT:
            if(stack->count < 1)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_CPU_IMP_OPER);  
            }

            out = stack_pop(stack);
            printf("out = %d\n", out);
            ++ip;
            break;

        case CMD_JMP:
            ip = get_arg(code, &ip, Regs) + 2; // transition cell number, taking into account the signature
            break;

        case CMD_HLT:
            exit(EXIT_SUCCESS);

        default:
            printf("ERROR: unknown operator.\n");
            exit(ERR_CPU_UNKNOWN_OPER);
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
        return ERR_CPU_BAD_SIGNATURE;
    }
    // if(code[1] != );

    return 0;
}

//=========================================================================

int get_arg(int* code, int* ip, regs_t* Regs)
{
    assert(code != NULL);
    assert(ip != NULL);

    int cmd = code[(*ip)++];
    int arg = 0;

    if(cmd == CMD_PUSH)
    {
        if(cmd & ARG_IMMED)
        {
            arg += code[*ip++];
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
        arg = code[*ip];
    }

    return arg;
}

//=========================================================================

int remove_whitespace(int* buffer, long count)
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
    logger_init(1, "asm.log");
    logger_set_level(INFO);

    stack_t stack;
    int init_size = 10;
    stack_init(&stack, init_size);
    
    FILE* binary_file = fopen("binary.out", "r");
    if (binary_file == NULL)
    {
        printf("ERROR: bad file read.\n");
        exit(ERR_CPU_BAD_FILE);
    }

    long count = count_symbols(binary_file);
    HANDLE_ERROR(count, ERR_CPU_BAD_PTR, "ERROR: pointer outside file.\n");

    int* buffer = (int*) calloc(count, sizeof(int));
    int ret = fill_buffer(binary_file, buffer, sizeof(int), count);
    HANDLE_ERROR(ret, ERR_CPU_BAD_READ, "ERROR: file read error.\n");

    fclose(binary_file);

    regs_t* Regs = (regs_t*) calloc(count_regs, sizeof(regs_t));
    regs_init(Regs);
    
    int new_count = remove_whitespace(buffer, count);
    int* code = (int*) realloc(buffer, new_count * sizeof(int));
    free(buffer);

    run(&stack, code, new_count, Regs);

    stack_dump(&stack);
    stack_destroy(&stack);

    logger_finalize(file);

    return 0;
}
