#include <stdio.h>
#include <stdlib.h>
#include <logger.h>
#include <onegin.h>
#include "cpu.h"


//=========================================================================

int decompile(char* code, int count, int* pos_labels, int found_labels)
{
    int real_count_labels = 0;
    if(!found_labels)
    {
        // this method is necessary in order not to count the immediate as command jmp
        fill_pos_labels(code, count, &real_count_label);
        return 0;
    }

    FILE* out = fopen("disasm.s", "w");
    if (out == NULL)
    {
        printf("ERROR: bad file read.\n");
        exit(ERR_BAD_FILE);
    }

    int ip = LEN_SIGNATURE;
    int idx = 0;
    int positipon = 0;
    while(ip < count)
    {
        if((code[ip] & CMD_MASK_1) == CMD_HLT)
        {
            fprintf(out, "hlt\n");
            break;
        }
        
        positipon = ip - LEN_SIGNATURE + 1; // line where label locates
        if(dasm_label_exist(pos_labels, real_count_labels, positipon))
        {
            fprintf(out, "l_%d\n", positipon);
            continue;
        }

        switch (code[ip] & CMD_MASK_1)
        {
        case CMD_PUSH:
            fprintf(out, "push ");
            dis_eval(out, code, &ip);
            ++ip;
            break;
        
        case CMD_POP:
            fprintf(out, "pop ");
            dis_eval(out, code, &ip);
            ++ip;
            break;

        case CMD_ADD:
            fprintf(out, "add\n");
            ++ip;
            break;

        case CMD_SUB:
            fprintf(out, "sub\n");
            ++ip;
            break;

        case CMD_MUL:
            fprintf(out, "mul\n");
            ++ip;
            break;

        case CMD_DIV:
            fprintf(out, "div\n");
            ++ip;
            break;

        case CMD_JMP:
            fprintf(out, "jmp l_");
            dis_eval(out, code, &ip);
            ++ip;
            break;

        case CMD_DUP:
            fprintf(out, "dup\n");
            ++ip;
            break;

        case CMD_OUT:
            fprintf(out, "out\n");
            ++ip;
            break;

        default:
            printf("LINE %d ERROR: unknown operator.\n", __LINE__);
            exit(ERR_UNKNOWN_OPER);
        }
    }
    fclose(out);

    return 0;
}

//=========================================================================
//TODO реализовать написание в файл имени метки, куда jmp
void dis_eval(FILE* out, char* code, int* ip)
{
    char cmd = code[*ip];
    
    if((cmd & CMD_MASK_1) == CMD_PUSH)
    {
        int arg_1 = 0;
        int arg_2 = 0;

        if(cmd & ARG_RAM)
        {
            ++(*ip);
            if(cmd & ARG_IMMED)
            {
                fprintf(out, "[%d ", code[*ip]);
                if(cmd & ARG_REG)
                {
                    ++(*ip);
                    fprintf(out, "+ ");
                    fprintf_reg(out, code, ip);
                    fprintf(out, "]\n");
                }

                return;                
            }
        }
        if(cmd & ARG_IMMED)
        {
            ++(*ip);
            fprintf(out, "%d\n", code[*ip]);

            return;
        }
        if(cmd & ARG_REG)
        {
            ++(*ip);
            fprintf_reg(out, code, ip);
            fprintf(out, "\n");

            return;
        }
    }
    else if((cmd & CMD_MASK_1) == CMD_POP)
    {
        if(cmd & ARG_RAM)
        {
            ++(*ip);
            if(cmd & ARG_IMMED)
            {
                fprintf(out, "[%d ", code[*ip]);
                if(cmd & ARG_REG)
                {
                    ++(*ip);
                    fprintf(out, "+ ");
                    fprintf_reg(out, code, ip);
                    fprintf(out, "]\n");
                }

                return;                
            }
        }
        if(cmd & ARG_REG)
        {
            ++(*ip);
            fprintf_reg(out, code, ip);
            fprintf(out, "\n");

            return;
        }
    }
    else if((cmd & CMD_MASK_1) == CMD_JMP)
    {
        ++(*ip);
        fprintf(out, "%d\n", code[*ip]);

        return;
    }

    printf("LINE %d ERROR: unknown argument.\n", __LINE__);
    exit(ERR_UNKNOWN_ARG);  
}

//=========================================================================

void fprintf_reg(FILE* out, char* code, int* ip)
{
    switch (code[*ip])
    {
    case REG_RAX:
        fprintf(out, "rax");
        break;
    
    case REG_RBX:
        fprintf(out, "rbx");
        break;

    case REG_RCX:
        fprintf(out, "rcx");
        break;

    case REG_RDX:
        fprintf(out, "rdx");
        break;

    default:
        printf("LINE %d ERROR: unknown argument.\n", __LINE__);
        exit(ERR_UNKNOWN_ARG);  
    }
}

//=========================================================================

int dasm_count_labels(char* code, int count)
{
    int count_labels = 0;
    for(int idx = 0; idx < count; ++idx)
    {
        if((code[idx] & CMD_MASK_1) == CMD_JMP)
        {
            ++count_labels;
        }
    }

    return count_labels;
}

//=========================================================================

void skip_arg(char* code, int* ip)
{
    char cmd = code[*ip];
    
    if((cmd & CMD_MASK_1) == CMD_PUSH)
    {
        if(cmd & ARG_RAM)
        {
            ++(*ip);
            if(cmd & ARG_IMMED)
            {
                if(cmd & ARG_REG)
                {
                    ++(*ip);
                }
                return;                
            }
        }
        if(cmd & ARG_IMMED)
        {
            ++(*ip);
            return;
        }
        if(cmd & ARG_REG)
        {
            ++(*ip);
            return;
        }
    }

    if((cmd & CMD_MASK_1) == CMD_POP)
    {
        if(cmd & ARG_RAM)
        {
            ++(*ip);
            if(cmd & ARG_IMMED)
            {
                if(cmd & ARG_REG)
                {
                    ++(*ip);
                }
                return;                
            }
        }
        if(cmd & ARG_REG)
        {
            ++(*ip);
            return;
        }
    }
}

//=========================================================================

void fill_pos_labels(char* code, int count, int* real_count_label)
{
    int ip = LEN_SIGNATURE;
    int idx = 0;

    while(ip < count)
    {
        if((code[ip] & CMD_MASK_1) == CMD_HLT)
        {
            break;
        }

        switch (code[ip] & CMD_MASK_1)
        {
        case CMD_PUSH:
            skip_arg(code, &ip);
            ++ip;
            break;
            
        case CMD_POP:
            skip_arg(code, &ip);
            ++ip;
            break;

        case CMD_ADD:
            ++ip;
            break;

        case CMD_SUB:
            ++ip;
            break;

        case CMD_MUL:
            ++ip;
            break;

        case CMD_DIV:
            ++ip;
            break;

        case CMD_JMP:
            ++ip;
            pos_labels[idx] = code[idx];
            ++(*real_count_label);
            break;

        case CMD_DUP:
            ++ip;
            break;

        case CMD_OUT:
            ++ip;
            break;

        default:
            printf("LINE %d ERROR: unknown operator.\n", __LINE__);
            exit(ERR_UNKNOWN_OPER);
        }
    }    
}

//=========================================================================

int dasm_label_exist(int* pos_labels, int count, int positiont)
{
    for(int idx = 0; idx < count; ++idx)
    {
        if(pos_labels[idx] == positiont)
        {
            return 1;
        }
    }
    return 0;
}

//=========================================================================

int main()
{
    logger_init(1, "dasm.log");
    logger_set_level(INFO);

    FILE* binary_file = fopen("binary.out", "r");
    if (binary_file == NULL)
    {
        printf("ERROR: bad file read.\n");
        exit(ERR_BAD_FILE);
    }

    int count = count_symbols(binary_file);
    HANDLE_ERROR(count, ERR_BAD_PTR, "ERROR: pointer outside file.\n");

    char* code = (char*) calloc(count, sizeof(char));
    int ret = fill_buffer(binary_file, code, sizeof(char), count);
    HANDLE_ERROR(ret, ERR_BAD_READ, "ERROR: file read error.\n");

    fclose(binary_file);

    int count_labels = dasm_count_labels(code, count);
    int pos_labels[count_labels];

    int found_labels = 0;
    decompile(code, count, pos_labels, found_labels);
    int found_labels = 1;
    decompile(code, count, pos_labels, found_labels);

    logger_finalize(file);

    return 0;
}
