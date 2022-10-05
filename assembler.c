#include <stdio.h>
#include <logger.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include "assembler.h"
// #include "../onegin/include/onegin.h"
// #include "../stack/include/stack.h"


//=========================================================================
int calculate(stack_t* stack, struct string_t* strings, int number_strings)
{
    elem_t addition = 0;
    elem_t subtraction = 0;
    elem_t multiplication = 0;
    elem_t division = 0;
    elem_t out = 0;

    for(int idx = 0; idx < number_strings; ++idx)
    {
        char* cmd = "";
        int count = 0; // number of symbols read

        sscanf(strings[idx].begin_string, " %s %n ", cmd, &count);

        int white_symbols = count_whitespace(strings[idx], count);
        int n = count + white_symbols;

        if(stricmp(cmd, "push") == 0)
        {
            elem_t val = 0;
            sscanf(strings[idx].begin_string + n, " %d ", &val);
            stack_push(stack, val);
        }
        else if(stricmp(cmd, "add") == 0)
        {
            if(stack->count < 2)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_ASM_IMP_OPER);
            }
            addition = stack_pop(stack) + stack_pop(stack);
            stack_push(stack, addition);
        }
        else if(stricmp(cmd, "sub") == 0)
        {
            if(stack->count < 2)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_ASM_IMP_OPER);
            }
            subtraction = -(stack_pop(stack) - stack_pop(stack));
            stack_push(stack, subtraction);         
        }
        else if(stricmp(cmd, "mul") == 0)
        {
            if(stack->count < 2)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_ASM_IMP_OPER);
            }
            multiplication = stack_pop(stack) * stack_pop(stack);
            stack_push(stack, multiplication);         
        }
        else if(stricmp(cmd, "div") == 0)
        {
            if(stack->count < 2)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_ASM_IMP_OPER);
            }

            elem_t rhs = stack_pop(stack);
            elem_t lhs = stack_pop(stack);
            if(is_zero(rhs))
            {
                printf("ERROR: division by zero.\n");
                exit(ERR_ASM_DIV_ZERO);
            }

            division = lhs / rhs;
            stack_push(stack, division);         
        }
        else if(stricmp(cmd, "out") == 0)
        {
            if(stack->count < 1)
            {
                printf("ERROR: impossible operation.\n");
                exit(ERR_ASM_IMP_OPER);  
            }
            out = stack_pop(stack);
            printf("out = %d\n", out);
        }
        else if(stricmp(cmd, "hlt") == 0)
        {
            break;
        }
        else
        {
            printf("ERROR: unknown operator.\n");
            exit(ERR_ASM_UNKNOWN_OPER);
        }

    }

    return 0;
}

//=========================================================================

int is_zero(double comparison)
{
    assert(!isnanf(comparison));

    return(fabs(comparison) < epsilon);
}

//=========================================================================

int count_whitespace(struct string_t str, int count)
{
    int white_symbols = 0;

    char* ptr_current_position = str.begin_string + count;
    while(isalpha(*ptr_current_position) == 0)
    {
        ++ptr_current_position;
        ++white_symbols;
    }

    return white_symbols;
}

//=========================================================================

int main()
{
    logger_init(1, "asm.log");
    logger_set_level(INFO);

    stack_t stack;
    int init_size = 4;
    stack_init(&stack, init_size);

    FILE* text = open_file("calculator.txt");
    if (text == NULL)
    {
        printf("ERROR: bad file read.\n");
        exit(ERR_ASM_BAD_FILE);
    }

    long count = count_symbols(text);
    HANDLE_ERROR(count, ERR_ASM_BAD_PTR, "ERROR: pointer outside file.\n");

    char* buffer = (char*) calloc(count, sizeof(char));
    int ret = fill_buffer(text, buffer, count);
    HANDLE_ERROR(ret, ERR_ASM_BAD_READ, "ERROR: file read error.\n");

    fclose(text);

    int number_strings = count_strings(buffer, count);
    struct string_t* strings = get_strings(buffer, count, number_strings);


    // TODO function calculate there

    stack_dump(&stack);
    stack_destroy(&stack);

    logger_finalize(file);

    return 0;
}
