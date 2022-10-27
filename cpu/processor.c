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

int run(stack_t* stack, char* code, int count, regs_t* Regs, char* RAM)
{
    assert(stack != NULL);
    assert(code != NULL);
    assert(Regs != NULL);    

    int ip = 0;

    char addition = 0;
    char subtraction = 0;
    char multiplication = 0;
    char division = 0;
    char out = 0;

    char lhs = 0;
    char rhs = 0;
    
    int ret = check_signature(code);
    HANDLE_ERROR(ret, ERR_BAD_SIGNATURE, "ERROR: incorrect signature.\n");

    ip += LEN_SIGNATURE;
    int arg = 0;
    while(ip < count)
    {
        switch (code[ip] & CMD_MASK_2)
        {
        case CMD_PUSH:
            arg = eval(code, &ip, Regs, RAM);
            stack_push(stack, arg);
            ++ip;
            break;

        case CMD_POP:
            if(code[ip] & ARG_RAM)
            {
                RAM[eval(code, &ip, Regs, RAM)] = stack_pop(stack);
                ++ip;
                break;
            }
            else if(code[ip] & ARG_REG)
            {
                Regs[eval(code, &ip, Regs, RAM) - 1].value = stack_pop(stack);
                ++ip;
                break;
            }

        case CMD_ADD:
            if(stack->count < 2)
            {
                printf("ERROR: empty stack.\n");
                exit(ERR_BAD_OPER);
            }

            addition = stack_pop(stack) + stack_pop(stack);
            stack_push(stack, addition);
            ++ip;
            break;

        case CMD_SUB:
            if(stack->count < 2)
            {
                printf("ERROR: empty stack.\n");
                exit(ERR_BAD_OPER);
            }

            rhs = stack_pop(stack);
            lhs = stack_pop(stack);
            subtraction = lhs - rhs;
            stack_push(stack, subtraction);
            ++ip;
            break;

        case CMD_MUL:
            if(stack->count < 2)
            {
                printf("ERROR: empty stack.\n");
                exit(ERR_BAD_OPER);
            }

            multiplication = stack_pop(stack) * stack_pop(stack);
            stack_push(stack, multiplication);
            ++ip;
            break;

        case CMD_DIV:
            if(stack->count < 2)
            {
                printf("ERROR: empty stack.\n");
                exit(ERR_BAD_OPER);
            }

            rhs = stack_pop(stack);
            lhs = stack_pop(stack);
            if(rhs == 0)
            {
                printf("ERROR: division by zero.\n");
                exit(ERR_DIV_ZERO);
            }

            division = lhs / rhs;
            stack_push(stack, division); 
            ++ip;
            break;

        case CMD_SQRT:
            if(stack->count < 1)
            {
                printf("ERROR: empty stack.\n");
                exit(ERR_BAD_OPER);  
            }

            out = stack_pop(stack);
            out = sqrt(out);   
            stack_push(stack, out);
            ++ip;
            break;

        case CMD_OUT:
            if(stack->count < 1)
            {
                printf("ERROR: empty stack.\n");
                exit(ERR_BAD_OPER);  
            }

            out = stack_pop(stack);
            printf("out = %d\n", out);
            ++ip;
            break;

        case CMD_DUP:
            if(stack->count < 1)
            {
                printf("ERROR: empty stack.\n");
                exit(ERR_BAD_OPER);  
            }
            out = stack_pop(stack);
            stack_push(stack, out);
            stack_push(stack, out);
            ++ip;
            break;

        case CMD_IN:
            arg = eval(code, &ip, Regs, RAM);
            stack_push(stack, arg);
            ++ip;
            break;

        case CMD_CALL:
            ip = eval(code, &ip, Regs, RAM) + LEN_SIGNATURE; // transition cell number, taking into account the signature           
            break;

        case CMD_RET:
            ip = Regs[REG_RAX - 1].value + LEN_SIGNATURE - 1; // The register contains the line number. We go to the team number
            break;

        case CMD_JMP:
            ip = eval(code, &ip, Regs, RAM) + LEN_SIGNATURE; // transition cell number, taking into account the signature
            break;

        case CMD_JB:
            if(stack->count < 2)
            {
                printf("ERROR: empty stack.\n");
                exit(ERR_BAD_OPER);
            }

            rhs = stack_pop(stack);
            lhs = stack_pop(stack);

            if(lhs < rhs)
            {
                ip = eval(code, &ip, Regs, RAM) + LEN_SIGNATURE;
                break;              
            }
            ip += 2;
            break;

        case CMD_JBE:
            if(stack->count < 2)
            {
                printf("ERROR: empty stack.\n");
                exit(ERR_BAD_OPER);
            }

            rhs = stack_pop(stack);
            lhs = stack_pop(stack);

            if(lhs <= rhs)
            {
                ip = eval(code, &ip, Regs, RAM) + LEN_SIGNATURE;
                break;           
            }
            ip += 2;
            break;

        case CMD_JA:
            if(stack->count < 2)
            {
                printf("ERROR: empty stack.\n");
                exit(ERR_BAD_OPER);
            }

            rhs = stack_pop(stack);
            lhs = stack_pop(stack);

            if(lhs > rhs)
            {
                ip = eval(code, &ip, Regs, RAM) + LEN_SIGNATURE;
                break;         
            }
            ip += 2;
            break;

        case CMD_JAE:
            if(stack->count < 2)
            {
                printf("ERROR: empty stack.\n");
                exit(ERR_BAD_OPER);
            }

            rhs = stack_pop(stack);
            lhs = stack_pop(stack);

            if(lhs >= rhs)
            {
                ip = eval(code, &ip, Regs, RAM) + LEN_SIGNATURE;
                break;          
            }
            ip += 2;
            break;

        case CMD_JE:
            if(stack->count < 2)
            {
                printf("ERROR: empty stack.\n");
                exit(ERR_BAD_OPER);
            }


            rhs = stack_pop(stack);
            lhs = stack_pop(stack);

            if(lhs == rhs)
            {
                ip = eval(code, &ip, Regs, RAM) + LEN_SIGNATURE;
                break;             
            }
            ip += 2;
            break;

        case CMD_JNE:
            if(stack->count < 2)
            {
                printf("ERROR: empty stack.\n");
                exit(ERR_BAD_OPER);
            }

            rhs = stack_pop(stack);
            lhs = stack_pop(stack);

            if(lhs != rhs)
            {
                ip = eval(code, &ip, Regs, RAM) + LEN_SIGNATURE;
                break;               
            }
            ip += 2;
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

int check_signature(char* code)
{
    assert(code != NULL);
    int len = sizeof(CP) / sizeof(char);

    if((memcmp(code, &CP, sizeof(CP)) != 0))
    {
        return ERR_BAD_SIGNATURE;
    }

    return 0;
}

//=========================================================================

int eval(char* code, int* ip, regs_t* Regs, char* RAM)
{
    assert(code != NULL);
    assert(ip != NULL);

    int cmd = code[*ip];
    int arg = 0;
    ++(*ip);

    if((cmd  & CMD_MASK_2) == CMD_PUSH)
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
    if((cmd == CMD_JMP) || (cmd == CMD_JB) || (cmd == CMD_JBE) || (cmd == CMD_JA) ||
       (cmd == CMD_JAE) || (cmd == CMD_JE) || (cmd == CMD_JNE) || (cmd == CMD_CALL))
    {
        arg = code[*ip];
    }
    if(cmd == CMD_IN)
    {
        arg = code[*ip];
    }
    if((cmd  & CMD_MASK_2) == CMD_POP)
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
                arg += Regs[code[*ip] - 1].value;
            }
            return arg;
        }
        if(cmd & ARG_REG)
        {
            arg += code[*ip]; // number of register
            if(cmd & ARG_RAM)
            {
                arg = Regs[arg - 1].value;
                return arg;
            }  
        }
    }

    return arg;
}

//=========================================================================

void cpu_dump(char* code, int count)
{
    char binary[8];
    char bits[3];
    int ip = LEN_SIGNATURE;

    while(ip < count)
    {   
        switch (code[ip] & CMD_MASK_2)
        {
        case CMD_PUSH:
            fill_bits(code[ip], bits);
            convert_binary(code[ip], binary);
            LOG("%d %x %s (%s #1)\n", code[ip], code[ip], binary, bits);
            ++ip;
            break;

        case CMD_POP:
            fill_bits(code[ip], bits);
            convert_binary(code[ip], binary);
            LOG("%d %x %s (%s #2)\n", code[ip], code[ip], binary, bits);
            ++ip;
            break;

        case CMD_ADD:
            convert_binary(code[ip], binary);
            LOG("%d %x %s (#3)\n", code[ip], code[ip], binary);
            ++ip;
            break;

        case CMD_SUB:
            convert_binary(code[ip], binary);
            LOG("%d %x %s (#4)\n", code[ip], code[ip], binary);
            ++ip;
            break;

        case CMD_MUL:
            convert_binary(code[ip], binary);
            LOG("%d %x %s (#5)\n", code[ip], code[ip], binary);
            ++ip;
            break;

        case CMD_DIV:
            convert_binary(code[ip], binary);
            LOG("%d %x %s (#6)\n", code[ip], code[ip], binary);
            ++ip;
            break;

        case CMD_OUT:
            convert_binary(code[ip], binary);
            LOG("%d %x %s (#7)\n", code[ip], code[ip], binary);
            ++ip;
            break;

        case CMD_HLT:
            convert_binary(code[ip], binary);
            LOG("%d %x %s (#8)\n", code[ip], code[ip], binary);
            break;

        case CMD_DUP:
            convert_binary(code[ip], binary);
            LOG("%d %x %s (#9)\n", code[ip], code[ip], binary);
            break;

        case CMD_JMP:
            convert_binary(code[ip], binary);
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

void fill_bits(char n, char* bits)
{
    bits[0] = (n & ARG_RAM)   ? 'M' : 'm';
    bits[1] = (n & ARG_REG)   ? 'R' : 'r';
    bits[2] = (n & ARG_IMMED) ? 'I' : 'i';
}

//=========================================================================

void convert_binary(char n, char* binary)
{
    for (int i = 0; i < 8; ++i)
    {
        binary[i] = (n & 0x80) ? '1' : '0';
        n <<= 1;
    }
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
