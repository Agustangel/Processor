#pragma once



enum cpu_error_names
{
    ERR_CPU_BAD_FILE     = -9,
    ERR_CPU_BAD_PTR      = -8,
    ERR_CPU_BAD_READ     = -7,
    ERR_CPU_IMP_OPER     = -6,
    ERR_CPU_DIV_ZERO     = -5,
    ERR_CPU_UNKNOWN_OPER = -4,
};

const unsigned int CP = 0xABADBABE;
