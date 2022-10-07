#include <stdio.h>
#include <logger.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include "processor.h"


//=========================================================================

int processor(stack_t* stack, int* code, int new_count)
{
    int addition = 0;
    int subtraction = 0;
    int multiplication = 0;
    int division = 0;
    int out = 0;

    if(code[0] != CP)
    {
        printf("ERROR: incorrect signature");
        exit(ERR_CPU_BAD_SIGNATURE);
    }
    
    for(int ip = 2; ip < new_count; ++ip)
    {
        switch (code[ip])
        {
        case CMD_PUSH:
            stack_push(stack, code[ip + 1]);
            ip += 2;
            break;

        case CMD_ADD:
            if(stack->count < 2)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_CPU_IMP_OPER);
            }

            addition = stack_pop(stack) + stack_pop(stack);
            stack_push(stack, addition);
            ip += 1;
            break;

        case CMD_SUB:
            if(stack->count < 2)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_CPU_IMP_OPER);
            }

            subtraction = -(stack_pop(stack) - stack_pop(stack));
            stack_push(stack, subtraction);
            ip += 1;
            break;

        case CMD_MUL:
            if(stack->count < 2)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_CPU_IMP_OPER);
            }

            multiplication = stack_pop(stack) * stack_pop(stack);
            stack_push(stack, multiplication);
            ip += 1;
            break;

        case CMD_DIV:
            if(stack->count < 2)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_CPU_IMP_OPER);
            }

            int rhs = stack_pop(stack);
            int lhs = stack_pop(stack);
            if(is_zero(rhs))
            {
                printf("ERROR: division by zero.\n");
                exit(ERR_CPU_DIV_ZERO);
            }

            division = lhs / rhs;
            stack_push(stack, division); 
            ip += 1;
            break;

        case CMD_OUT:
            if(stack->count < 1)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_CPU_IMP_OPER);  
            }

            out = stack_pop(stack);
            printf("out = %d\n", out);
            ip += 1;
            break;

        case CMD_HLT:
            //stop calculate

        default:
            printf("ERROR: unknown operator.\n");
            exit(ERR_CPU_UNKNOWN_OPER);
            break;
        }
    }
    return 0;
}

//=========================================================================

int remove_whitespace(int* buffer, long count)
{
    assert(buffer != NULL);

    int new_idx = 0;
    for(int idx = 0; idx < count; ++idx)
    {
        if(isdigit(buffer[idx]))
        {
            ++new_idx;
        }
    }
    return new_idx;
}

//=========================================================================

int main()
{
    logger_init(1, "asm.log");
    logger_set_level(INFO);

    stack_t stack;
    int init_size = 4;
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

    int new_count = remove_whitespace(buffer, count);
    int* code = (int*) realloc(buffer, new_count * sizeof(int));
    free(buffer);

    stack_dump(&stack);
    stack_destroy(&stack);

    logger_finalize(file);

    return 0;
}
