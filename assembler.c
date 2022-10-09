#include <stdio.h>
#include <logger.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include "assembler.h"


//=========================================================================
int compile(struct string_t* strings, int number_strings, label_t* labels, int number_labels, int fill_labels)
{
    FILE* out = fopen("binary.out", "w");
    if (out == NULL)
    {
        printf("ERROR: bad file read.\n");
        exit(ERR_ASM_BAD_FILE);
    }

    fprintf(out, "%d %d", CP, number_strings);

    char cmd[max_size];
    char label[max_size];
    int count_label = 0;
    int count = 0; // number of symbols read

    for(int idx = 0; idx < number_strings; ++idx)
    {
        if(fill_labels)
        {
            sscanf(strings[idx].begin_string, "%s%n", cmd, &count); //
            printf("IDX = %d; cmd = %s\n", idx, cmd);

            int white_symbols = count_whitespace(strings[idx], count);
            int n = count + white_symbols;

            if(strcmp(cmd, "push") == 0)
            {
                char str[max_size];
                int val = 0;
                sscanf(strings[idx].begin_string + n, "%s", str);
 
                val = atoi(str);
                fprintf(out, "%d%d", CMD_PUSH, val);
            }
            else if(strcmp(cmd, "add") == 0)
            {
                fprintf(out, "%d", CMD_ADD);
            }
            else if(strcmp(cmd, "sub") == 0)
            {
                fprintf(out, "%d", CMD_SUB);       
            }
            else if(strcmp(cmd, "mul") == 0)
            {
                fprintf(out, "%d", CMD_MUL);         
            }
            else if(strcmp(cmd, "div") == 0)
            {
                fprintf(out, "%d", CMD_DIV);        
            }
            else if(strcmp(cmd, "jmp") == 0)
            {
                char str[max_size];
                int val = 0;
                sscanf(strings[idx].begin_string + n, "%s", str);
                
                for(int idx = 0; idx < number_labels; ++idx)
                {
                    if(strcmp(str, labels[idx].name))
                    {
                        val = labels[idx].value;
                    }
                }
                fprintf(out, "%d%d", CMD_JMP, val);                
            }
            else if(strcmp(cmd, "dup") == 0)
            {
                fprintf(out, "%d%d", CMD_DUP, CMD_DUP);
            }
            else if(strcmp(cmd, "out") == 0)
            {
                fprintf(out, "%d", CMD_OUT);
            }
            else if(strcmp(cmd, "hlt") == 0)
            {
                fprintf(out, "%d", CMD_HLT);
                break;
            }
            else
            {
                if(!label_exist(labels, number_labels, cmd))
                {
                    printf("LINE %d ERROR: unknown operator.\n", __LINE__);
                    exit(ERR_ASM_UNKNOWN_OPER);
                }
            }
        }
        else
        {
            sscanf(strings[idx].begin_string, "%s", label); // добавить проверку, что размера массива хватило
            
            char* tmp_ptr = label;
            char* ptr_colon = strchr(label, ':');
            if(ptr_colon == NULL)
            {
                continue;
            }

            if(*(ptr_colon + 1) != '\0')
            {
                printf("LINE %d ERROR: unknown operator.\n", __LINE__);
                exit(ERR_ASM_UNKNOWN_OPER);
            }
            while(tmp_ptr < ptr_colon)
            {
                if(!isalnum(*tmp_ptr))
                {
                    printf("LINE %d ERROR: unknown operator.\n", __LINE__);
                    exit(ERR_ASM_UNKNOWN_OPER);                    
                }
                ++tmp_ptr;
            }

            labels[count_label].name = label;
            labels[count_label].value = idx;
            ++count_label;
        }
    }
    fclose(out);

    return 0;
}

//=========================================================================

int label_exist(label_t* labels, int number_labels, char* cmd)
{
    for(int idx = 0; idx < number_labels; ++idx)
    {
        if(strcmp(cmd, labels[idx].name))
        {
            return 1;
        }
    }

    return 0;    
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

int count_labels(struct string_t* strings, int number_strings)
{
    int count_lab = 0;
    char cmd[max_size];

    for(int idx = 0; idx < number_strings; ++idx)
    {
        sscanf(strings[idx].begin_string, "%s", cmd);
        if(strcmp(cmd, "jmp") == 0)
        {
            ++count_lab;
        }
    }

    return count_lab;
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
    int ret = fill_buffer(text, buffer, sizeof(char), count);
    HANDLE_ERROR(ret, ERR_ASM_BAD_READ, "ERROR: file read error.\n");

    fclose(text);

    int number_strings = count_strings(buffer, count);
    struct string_t* strings = get_strings(buffer, count, number_strings);

    int number_labels = count_labels(strings, number_strings);
    label_t* labels = (label_t*) calloc(number_labels, sizeof(label_t));

    int fill_labels = 0; 
    compile(strings, number_strings, labels, number_labels, fill_labels); // the first compile
    fill_labels = 1;
    compile(strings, number_strings, labels, number_labels, fill_labels); // the second compile

    logger_finalize(file);

    return 0;
}
