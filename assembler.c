#include <stdio.h>
#include <logger.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include "assembler.h"


//=========================================================================
int calculate(struct string_t* strings, int number_strings)
{
    FILE* out = fopen("binary.out", "w");
    if (out == NULL)
    {
        printf("ERROR: bad file read.\n");
        exit(ERR_ASM_BAD_FILE);
    }

    fprintf(out, "%p %d", CD, number_strings);

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
            fprintf(out, "%d %d", CMD_PUSH, val);
        }
        else if(stricmp(cmd, "add") == 0)
        {
            fprintf(out, "%d", CMD_ADD);
        }
        else if(stricmp(cmd, "sub") == 0)
        {
            fprintf(out, "%d", CMD_SUB);       
        }
        else if(stricmp(cmd, "mul") == 0)
        {
            fprintf(out, "%d", CMD_MUL);         
        }
        else if(stricmp(cmd, "div") == 0)
        {
            fprintf(out, "%d", CMD_DIV);        
        }
        else if(stricmp(cmd, "out") == 0)
        {
            fprintf(out, "%d", CMD_OUT);
        }
        else if(stricmp(cmd, "hlt") == 0)
        {
            fprintf(out, "%d", CMD_HLT);
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

    FILE* text = open_file("initial.s");
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

    logger_finalize(file);

    return 0;
}
