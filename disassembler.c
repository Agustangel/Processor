#include <stdio.h>
#include <logger.h>
#include <onegin.h>
#include "cpu.h"



//=========================================================================

int decompile(int* code, int new_count)
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
        switch (code[ip])
        {
        case CMD_PUSH:
            fwrite("push ", sizeof(char), LEN_PUSH, out);
            ++ip;
            break;
        
        case CMD_POP:
            fwrite("pop ", sizeof(char), LEN_POP, out);
            ++ip;
            break;

        case CMD_ADD:
            fwrite("add\n", sizeof(char), LEN_ADD, out);
            ++ip;
            break;

        case CMD_SUB:
            fwrite("sub\n", sizeof(char), LEN_SUB, out);
            ++ip;
            break;

        case CMD_MUL:
            fwrite("mul\n", sizeof(char), LEN_MUL, out);
            ++ip;
            break;

        case CMD_DIV:
            fwrite("div\n", sizeof(char), LEN_DIV, out);
            ++ip;
            break;

        case CMD_JMP:
            fwrite("jmp ", sizeof(char), LEN_JMP, out);
            ++ip;
            break;

        case CMD_DUP:
            fwrite("dup\n", sizeof(char), LEN_DUP, out);
            ++ip;
            break;

        case CMD_OUT:
            fwrite("out\n", sizeof(char), LEN_OUT, out);
            ++ip;
            break;

        case CMD_HLT:
            fwrite("hlt\n", sizeof(char), LEN_HLT, out);
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
void dis_eval(FILE* out, char* code, int* ip, label_t* labels, int number_labels)
{
    if(code[*ip] == CMD_PUSH)
    {
        ++(*ip);

    }
    else if(code[*ip] == CMD_POP)
    {
        ++(*ip);
        switch (code[*ip])
        {
        case REG_RAX:
            fwrite("rax\n", sizeof(char), LEN_REG, out);
            break;
        
        case REG_RBX:
            fwrite("rbx\n", sizeof(char), LEN_REG, out);
            break;

        case REG_RCX:
            fwrite("rcx\n", sizeof(char), LEN_REG, out);
            break;

        case REG_RDX:
            fwrite("rdx\n", sizeof(char), LEN_REG, out);
            break;

        default:
            printf("LINE %d ERROR: unknown argument.\n", __LINE__);
            exit(ERR_UNKNOWN_ARG);  
        }
    }
    else if(code[*ip] == CMD_JMP)
    {
        ++(*ip);
        for(int idx = 0; idx < number_labels; ++idx)
        {
            if(*ip == labels[idx].value)
            {
                int len = srtlen(labels[idx].name);
                fwrite(labels[idx].name ":\n", sizeof(char), len, out);

                return;
            }
        }

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



    logger_finalize(file);

    return 0;
}
