#include <stdio.h>
#include <logger.h>
#include <onegin.h>
#include <stack.h>
#include "cpu.h"



int main()
{
    logger_init(1, "cpu.log");
    logger_set_level(INFO);

    stack_t stack;
    int init_size = 10;
    stack_init(&stack, init_size); 


    FILE* binary_file = fopen("binary.out", "r");
    if (binary_file == NULL)
    {
        printf("ERROR: bad file read.\n");
        exit(ERR_BAD_FILE);
    }

    long count = count_symbols(binary_file);
    HANDLE_ERROR(count, ERR_BAD_PTR, "ERROR: pointer outside file.\n");

    char* code = (char*) calloc(count, sizeof(char));
    int ret = fill_buffer(binary_file, code, sizeof(char), count);
    HANDLE_ERROR(ret, ERR_BAD_READ, "ERROR: file read error.\n");

    fclose(binary_file);

    regs_t* Regs = (regs_t*) calloc(count_regs, sizeof(regs_t));
    regs_init(Regs);
    
    char* RAM = (char*) malloc(RAM_SIZE * sizeof(char));

    run(&stack, code, count, Regs, RAM);
    cpu_dump(code, count);

    free(Regs);
    free(code);
    free(RAM);

    stack_destroy(&stack);
    logger_finalize(file);

    return 0;
}
