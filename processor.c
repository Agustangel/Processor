#include <stdio.h>
#include <logger.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include "processor.h"


//=========================================================================

int processor(FILE* file)
{

}

//=========================================================================
// TODO сделать функцию, удаляющую в буфере пробемы и \n
int remove_whitespace(char* buffer)
{

}

//=========================================================================

int main()
{
    FILE* binary_file = fopen(binary.out, "r");
    if (binary_file == NULL)
    {
        printf("ERROR: bad file read.\n");
        exit(ERR_CPU_BAD_FILE);
    }

    long count = count_symbols(binary_file);
    HANDLE_ERROR(count, ERR_CPU_BAD_PTR, "ERROR: pointer outside file.\n");

    char* buffer = (char*) calloc(count, sizeof(char));
    int ret = fill_buffer(binary_file, buffer, count);
    HANDLE_ERROR(ret, ERR_CPU_BAD_READ, "ERROR: file read error.\n");

    fclose(binary_file);

    // TODO сделать буфер из int(double?)
    
    return 0;
}
