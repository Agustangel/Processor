#include <stdio.h>
#include <logger.h>
#include "../onegin/include/onegin.h"
#include "../stack/include/stack.h"


int calculate(stack_t* stack, string_t* strings, int number_strings)
{
    for(int idx = 0; idx < number_strings; ++idx)
    {
        char* cmd = "";
        int count = 0; // number of symbols read
        int white_symbols = 0;     // number of whitespace sympols -> // TODO определить кол-во пробельных символов

        sscanf(strings[idx].begin_string, %s, %n, cmd, &count);
        int n = count + white_symbols;

        if(stricmp(cmd, "push") == 0)
        {
            int val = 0;
            sscanf(strings[idx].begin_string + n, %d, &val);
            stack_push(stack, val);
        }
        else if(stricmp(cmd, "add") == 0)
        {
            
        }

    }

    return 0;
}
int main()
{
    logger_init(1, "asm.log");
    logger_set_level(INFO);

    stack_t stack;
    int init_size = 4;
    stack_init(&stack, init_size);

    FILE* text = open_file("calculator.txt");
    if (text == NULL)
    {
        printf("ERROR: bad file read.\n");
        exit(ERR_BAD_FILE);
    }

    long count = count_symbols(text);
    HANDLE_ERROR(count, ERR_BAD_PTR, "ERROR: pointer outside file.\n");

    char* buffer = (char*) calloc(count, sizeof(char));
    int ret = fill_buffer(text, buffer, count);
    HANDLE_ERROR(ret, ERR_BAD_READ, "ERROR: file read error.\n");

    fclose(text);

    int number_strings = count_strings(buffer, count);
    struct string_t* strings = get_strings(buffer, count, number_strings);


    // TODO function calculate there

    stack_dump(&stack);
    stack_destroy(&stack);

    logger_finalize(file);

    return 0;
}
