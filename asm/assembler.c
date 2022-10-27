#include <stdio.h>
#include <logger.h>
#include <onegin.h>
#include <stack.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "cpu.h"


//=========================================================================

int compile(struct string_t* strings, int number_strings, label_t* labels, int* number_labels, int fill_labels)
{
    int ip = 0;
    char cmd [max_size];

    int count = 0;         // number of symbols read
    int white_symbols = 0; // number of whitespace symbols
    int n = 0;             // n = count + white_symbols

    if(!fill_labels)
    {
        int count_label = 0;
        int position = 0;

        for(int idx = 0; idx < number_strings; ++idx)
        {
            fill_label(strings[idx], labels, &count_label, &position);
        }
        *number_labels = count_label;

        return 0;
    }

    int len_code = number_strings * 2 + LEN_SIGNATURE;
    char* code = (char*) calloc(len_code, sizeof(char));

    memcpy(code, &CP, sizeof(CP));
    ip += LEN_SIGNATURE;
    
    for(int idx = 0; idx < number_strings; ++idx)
    {
        sscanf(strings[idx].begin_string, "%s%n", cmd, &count); //

        if(strcmp(cmd, "push") == 0)
        {
            code[ip] = CMD_PUSH;
            get_args(strings[idx], code, &ip, labels, *number_labels);
        }
        else if(strcmp(cmd, "pop") == 0)
        {
            code[ip] = CMD_POP;
            get_args(strings[idx], code, &ip, labels, *number_labels);
        }
        else if(strcmp(cmd, "add") == 0)
        {
            code[ip++] = CMD_ADD;
        }
        else if(strcmp(cmd, "sub") == 0)
        {
            code[ip++] = CMD_SUB;       
        }
        else if(strcmp(cmd, "mul") == 0)
        {
            code[ip++] = CMD_MUL;         
        }
        else if(strcmp(cmd, "div") == 0)
        {
            code[ip++] = CMD_DIV;        
        }
        else if(strcmp(cmd, "sqrt") == 0)
        {
            code[ip++] = CMD_SQRT;
        }
        else if(strcmp(cmd, "in") == 0)
        {
            code[ip++] = CMD_IN;
            get_args(strings[idx], code, &ip, labels, *number_labels);
        }
        else if(strcmp(cmd, "call") == 0)
        {
            code[ip++] = CMD_CALL;
            get_args(strings[idx], code, &ip, labels, *number_labels);
        }
        else if(strcmp(cmd, "ret") == 0)
        {
            code[ip++] = CMD_RET;
        }
        else if(strcmp(cmd, "jmp") == 0)
        {
            code[ip++] = CMD_JMP;
            get_args(strings[idx], code, &ip, labels, *number_labels);           
        }
        else if(strcmp(cmd, "jb") == 0)
        {
            code[ip++] = CMD_JB;
            get_args(strings[idx], code, &ip, labels, *number_labels);           
        }
        else if(strcmp(cmd, "jbe") == 0)
        {
            code[ip++] = CMD_JBE;
            get_args(strings[idx], code, &ip, labels, *number_labels);           
        }
        else if(strcmp(cmd, "ja") == 0)
        {
            code[ip++] = CMD_JA;
            get_args(strings[idx], code, &ip, labels, *number_labels);           
        }
        else if(strcmp(cmd, "jae") == 0)
        {
            code[ip++] = CMD_JAE;
            get_args(strings[idx], code, &ip, labels, *number_labels);           
        }
        else if(strcmp(cmd, "je") == 0)
        {
            code[ip++] = CMD_JE;
            get_args(strings[idx], code, &ip, labels, *number_labels);           
        }
        else if(strcmp(cmd, "jne") == 0)
        {
            code[ip++] = CMD_JNE;
            get_args(strings[idx], code, &ip, labels, *number_labels);           
        }
        else if(strcmp(cmd, "dup") == 0)
        {
            code[ip++] = CMD_DUP;
        }
        else if(strcmp(cmd, "out") == 0)
        {
            code[ip++] = CMD_OUT;
        }
        else if(strcmp(cmd, "hlt") == 0)
        {
            code[ip++] = CMD_HLT;
        }
        else
        {
            if(!label_exist(labels, *number_labels, cmd))
            {
                printf("LINE %d ERROR: unknown operator.\n", __LINE__);
                exit(ERR_UNKNOWN_OPER);
            }
        }
    }

    FILE* out = fopen("binary.out", "w");
    if (out == NULL)
    {
        printf("ERROR: bad file read.\n");
        exit(ERR_BAD_FILE);
    }
    int ret = fwrite(code, sizeof(char), number_strings * 2 + LEN_SIGNATURE, out);
    fclose(out);

    return 0;
}

//=========================================================================

void get_args(struct string_t string, char* code, int* ip, label_t* labels, int number_labels)
{
    int count = 0; // number of symbols read
    int len = 0;   // len of string read
    int val = 0;
    
    char cmd[max_size];
    char str[max_size];

    sscanf(string.begin_string, "%s%n", cmd, &count);

    int white_symbols = count_whitespace(string, count);
    int n = count + white_symbols;

    if(strcmp(cmd, "push") == 0)
    {
        if(sscanf(string.begin_string + n - 1, "%d", &val))
        {
            get_immed(val, code, ip);
            return;
        }
        else
        {
            sscanf(string.begin_string + n, "%s", str);
            len = strlen(str);

            if((str[0] == 'r') && (len == 3))
            {
                get_reg(str, code, ip);             
                return;
            }
            else if((str[0] == '[') && (str[len - 1] == ']'))
            {
                get_ram(string, code, ip, &n);
                return;
            }
            else
            {
                printf("LINE %d ERROR: unknown argument.\n", __LINE__);
                exit(ERR_UNKNOWN_ARG);                 
            }      
        }
    }
    else if((strcmp(cmd, "jmp") == 0) || (strcmp(cmd, "jb") == 0)  || (strcmp(cmd, "jbe") == 0) ||
            (strcmp(cmd, "ja") == 0)  || (strcmp(cmd, "jae") == 0) || (strcmp(cmd, "je") == 0)  ||
            (strcmp(cmd, "jne") == 0) || (strcmp(cmd, "call") == 0))
    {
        sscanf(string.begin_string + n, "%s", str);
                
        for(int idx = 0; idx < number_labels; ++idx)
        {
            if(strcmp(str, labels[idx].name) == 0)
            {
                val = labels[idx].value;
                code[*ip] = val;
                ++(*ip);
            }
        }
    }
    else if(strcmp(cmd, "pop") == 0)
    {
        sscanf(string.begin_string + n, "%s", str);
        len = strlen(str);

        if((str[0] == 'r') && (len == 3))
        {
            get_reg(str, code, ip);
            return;
        }
        else if((str[0] == '[') && (str[len - 1] == ']'))
        {
            get_ram(string, code, ip, &n);
            return;
        }
        else
        {
            printf("LINE %d ERROR: unknown argument.\n", __LINE__);
            exit(ERR_UNKNOWN_ARG);                 
        }      
    }
    else if(strcmp(cmd, "in") == 0)
    {
        scanf("%d", &val);
        code[*ip] = val;
        ++(*ip);
    }
}

//=========================================================================

void get_immed(int val, char* code, int* ip)
{
    code[*ip] |= ARG_IMMED;
    ++(*ip);
    code[*ip] = val;
    ++(*ip);
}

//=========================================================================

void get_reg(char* str, char* code, int* ip)
{
    code[*ip] |= ARG_REG;
    ++(*ip);

    if(strcmp(str, "rax") == 0)
    {
        code[*ip] = REG_RAX;
        ++(*ip);
    }
    else if(strcmp(str, "rbx") == 0)
    {
        code[*ip] = REG_RBX;
        ++(*ip);
    }
    else if(strcmp(str, "rcx") == 0)
    {
        code[*ip] = REG_RCX;
        ++(*ip);
    }
    else if(strcmp(str, "rdx") == 0)
    {
        code[*ip] = REG_RDX;
        ++(*ip);
    }
    else
    {
        printf("LINE %d ERROR: unknown argument.\n", __LINE__);
        exit(ERR_UNKNOWN_ARG);                      
    }
}

//=========================================================================

void get_ram(struct string_t string, char* code, int* ip, int* n)
{
    int count = 0;          // number of symbols read
    int white_symbols = 0;  // number of whitespace symbols
    int val = 0;
    
    char str[max_size];

    code[*ip] |= ARG_RAM;
    ++(*ip);
    ++(*n);

    sscanf(string.begin_string + *n, "%s", str);
    int len = strlen(str);

    if(sscanf(string.begin_string + *n, "%d%n", &val, &count))
    {
        *n += count;
        white_symbols = count_whitespace(string, *n);
        *n += white_symbols;

        get_immed(val, code, ip);

        if(*(string.begin_string + *n) == '+')
        {
            ++(*ip);
            ++(*n);
            white_symbols = count_whitespace(string, *n);
            *n += white_symbols;

            if(sscanf(string.begin_string + *n, "%d", &val))
            {
                get_immed(val, code, ip);
                return;
            }

            sscanf(string.begin_string + *n, "%s", str);
            len = strlen(str);   

            if((str[0] == 'r') && (len == 3))
            {
                get_reg(str, code, ip);
                return;
            }
            else
            {
                printf("LINE %d ERROR: unknown argument.\n", __LINE__);
                exit(ERR_UNKNOWN_ARG);                      
            }    
        }
    }
    else if((str[0] == 'r') && (len == 3))
    {
        *n += len;
        white_symbols = count_whitespace(string, *n);
        *n += white_symbols;

        get_reg(str, code, ip);

        if(*(string.begin_string + *n) == '+')
        {
            ++(*ip);
            ++(*n);
            white_symbols = count_whitespace(string, *n);
            *n += white_symbols;

            if(sscanf(string.begin_string + *n, "%d", &val))
            {
                get_immed(val, code, ip);
                swap_arg(code, ip);
                return;
            }

            sscanf(string.begin_string + *n, "%s", str);
            len = strlen(str);   

            if((str[0] == 'r') && (len == 3))
            {
                get_reg(str, code, ip);
                return;
            }
            else
            {
                printf("LINE %d ERROR: unknown argument.\n", __LINE__);
                exit(ERR_UNKNOWN_ARG);                      
            }  
        }
    }
    else
    {
        printf("LINE %d ERROR: unknown argument.\n", __LINE__);
        exit(ERR_UNKNOWN_ARG);                      
    }
}

//=========================================================================

void swap_arg(char* code, int* ip)
{
    int tmp = code[*ip];
    code[*ip] = code[(*ip) - 1];
    code[(*ip) - 1] = tmp;
}

//=========================================================================

int fill_label(struct string_t string, label_t* labels, int* count_label, int* position)
{
    char cmd [max_size];
    int count = 0;
    sscanf(string.begin_string, "%s%n", cmd, &count); //

    if(is_label(cmd))
    {
        char* label = (char*) calloc(strlen(cmd), sizeof(char));
        sscanf(string.begin_string, "%s", label);

        labels[*count_label].name = label;
        labels[*count_label].value = *position; // code[*position] is number next instruction
        
        ++(*count_label);
    }
    else if(strcmp(cmd, "push") == 0)
    {
        ++(*position);
        asm_skip_arg(string, cmd, count, position);
    }
    else if(strcmp(cmd, "pop") == 0)
    {
        ++(*position);
        asm_skip_arg(string, cmd, count, position);  
    }
    else if(strcmp(cmd, "add") == 0)
    {
        ++(*position);
    }
    else if(strcmp(cmd, "sub") == 0)
    {
        ++(*position);
    }
    else if(strcmp(cmd, "mul") == 0)
    {
        ++(*position);
    }
    else if(strcmp(cmd, "div") == 0)
    {
        ++(*position);
    }
    else if(strcmp(cmd, "sqrt") == 0)
    {
        ++(*position);
    }
    else if(strcmp(cmd, "in") == 0)
    {
        ++(*position);
        asm_skip_arg(string, cmd, count, position);
    }
    else if(strcmp(cmd, "call") == 0)
    {
        ++(*position);
        asm_skip_arg(string, cmd, count, position);
    }
    else if(strcmp(cmd, "ret") == 0)
    {
        ++(*position);
    }
    else if(strcmp(cmd, "jmp") == 0)
    {
        ++(*position);
        asm_skip_arg(string, cmd, count, position);
    }
    else if(strcmp(cmd, "jb") == 0)
    {
        ++(*position);
        asm_skip_arg(string, cmd, count, position);
    }
    else if(strcmp(cmd, "jbe") == 0)
    {
        ++(*position);
        asm_skip_arg(string, cmd, count, position);
    }
    else if(strcmp(cmd, "ja") == 0)
    {
        ++(*position);
        asm_skip_arg(string, cmd, count, position);
    }
    else if(strcmp(cmd, "jae") == 0)
    {
        ++(*position);
        asm_skip_arg(string, cmd, count, position);
    }
    else if(strcmp(cmd, "je") == 0)
    {
        ++(*position);
        asm_skip_arg(string, cmd, count, position);
    }
    else if(strcmp(cmd, "jne") == 0)
    {
        ++(*position);
        asm_skip_arg(string, cmd, count, position);
    }
    else if(strcmp(cmd, "dup") == 0)
    {
        ++(*position);
    }
    else if(strcmp(cmd, "out") == 0)
    {
        ++(*position);
    }
    else if(strcmp(cmd, "hlt") == 0)
    {
        ++(*position);
    }
}

//=========================================================================

void asm_skip_arg(struct string_t string, char* cmd, int count, int* position)
{
    int val = 0;
    char str[max_size];

    int white_symbols = count_whitespace(string, count);
    int n = count + white_symbols;

    if(strcmp(cmd, "push") == 0)
    {
        if(sscanf(string.begin_string + n, "%d", &val))
        {
            ++(*position);
            return;
        }
        else
        {
            sscanf(string.begin_string + n, "%s", str);
            int len = strlen(str);
            ++(*position);             
            return;
            
            if((str[0] == '[') && (str[len - 1] == ']'))
            {
                ++n;
                sscanf(string.begin_string + n, "%s%d", str, &count);
                
                n += count;
                white_symbols = count_whitespace(string, n);
                n += white_symbols;

                if(*(string.begin_string + n) == '+')
                {
                    ++(*position);
                }

                return;
            }     
        }
    }
    if(strcmp(cmd, "pop") == 0)
    {
        sscanf(string.begin_string + n, "%s", str);
        int len = strlen(str);
        ++(*position);             
        return;
            
        if((str[0] == '[') && (str[len - 1] == ']'))
        {
            ++n;
            sscanf(string.begin_string + n, "%s%d", str, &count);
            
            n += count;
            white_symbols = count_whitespace(string, n);
            n += white_symbols;

            if(*(string.begin_string + n) == '+')
            {
                ++(*position);
            }
                
            return;
        }
    }
    if((strcmp(cmd, "in") == 0))
    {
        ++(*position);
        return;
    }
    if((strcmp(cmd, "jmp") == 0) || (strcmp(cmd, "jb") == 0)  || (strcmp(cmd, "jbe") == 0)||
       (strcmp(cmd, "ja") == 0)  || (strcmp(cmd, "jae") == 0) || (strcmp(cmd, "je") == 0) ||
       (strcmp(cmd, "jne") == 0) || (strcmp(cmd, "call") == 0))
    {
        ++(*position);
        return;
    }
    
}

//=========================================================================

int is_label(char* cmd)
{
    char* tmp_ptr = cmd;
    char* ptr_colon = strchr(cmd, ':');
    if(ptr_colon == NULL)
    {
        return 0;
    }

    if(*(ptr_colon + 1) != '\0')
    {
        printf("LINE %d ERROR: unknown operator.\n", __LINE__);
        exit(ERR_UNKNOWN_OPER);
    }
    while(tmp_ptr < ptr_colon)
    {
        if(!isalnum(*tmp_ptr))
        {
            printf("LINE %d ERROR: unknown operator.\n", __LINE__);
            exit(ERR_UNKNOWN_OPER);                    
        }
        ++tmp_ptr;
    }

    return 1;
}

//=========================================================================

int label_exist(label_t* labels, int number_labels, char* cmd)
{
    for(int idx = 0; idx < number_labels; ++idx)
    {
        if(strcmp(cmd, labels[idx].name) == 0)
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
    while(isalnum(*ptr_current_position) == 0)
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
        if((strcmp(cmd, "jmp") == 0) || (strcmp(cmd, "jb") == 0)  || (strcmp(cmd, "jbe") == 0) ||
           (strcmp(cmd, "ja") == 0)  || (strcmp(cmd, "jae") == 0) || (strcmp(cmd, "je") == 0)  ||
           (strcmp(cmd, "jne") == 0) || (strcmp(cmd, "call") == 0))
        {
            ++count_lab;
        }
    }

    return count_lab;
}
