#pragma once

#include "../onegin/include/onegin.h"
#include "../stack/include/stack.h"


enum cpu_error_names
{
    ERR_CPU_BAD_FILE  = -9,
    ERR_CPU_BAD_PTR   = -8,
    ERR_CPU_BAD_READ  = -7,
    ERR_CPU_IMP_OPER  = -6,
    ERR_CPU_DIV_ZERO  = -5
};

static const double epsilon = 0.00001;


int calculate(stack_t* stack, struct string_t* strings, int number_strings);
int is_zero(double comparison);
int count_whitespace(struct string_t str, int count);
