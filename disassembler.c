#include <stdio.h>
#include <stdlib.h>
#include <logger.h>
#include <onegin.h>
#include "cpu.h"


// TODO написать функцию dump для asm, cpu, dasm
//=========================================================================

int decompile(char* code, int new_count)
{
    FILE* out = fopen("disasm.s", "w");
    if (out == NULL)
    {
        printf("ERROR: bad file read.\n");
        exit(ERR_BAD_FILE);
    }

    int ip = count_signature;
    while(ip < new_count)
    {
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
            fprintf(out, "jmp ");
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

        case CMD_HLT:
            fprintf(out, "hlt\n");
            ++ip;
            break;

        default:
            printf("LINE %d ERROR: unknown operator.\n", __LINE__);
            exit(ERR_UNKNOWN_OPER);
        }
    }

    fclose(out);
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
        ++(*ip);
        fprintf_reg(out, code, ip);
        fprintf(out, "\n");
    }
    else if((cmd & CMD_MASK_1) == CMD_JMP)
    {
        ++(*ip);
        fprintf(out, "l_%d\n", *ip);

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

    long count = count_symbols(binary_file);
    HANDLE_ERROR(count, ERR_BAD_PTR, "ERROR: pointer outside file.\n");

    char* buffer = (char*) calloc(count, sizeof(char));
    int ret = fill_buffer(binary_file, buffer, sizeof(char), count);
    HANDLE_ERROR(ret, ERR_BAD_READ, "ERROR: file read error.\n");

    fclose(binary_file);
    
    int new_count = remove_whitespace(buffer, count);
    char* code = (char*) realloc(buffer, new_count * sizeof(char));
    free(buffer);

    decompile(code, new_count);

    logger_finalize(file);

    return 0;
}
