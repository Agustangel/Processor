#include <stdio.h>
#include <logger.h>
#include <onegin.h>
#include <stack.h>
#include "cpu.h"
#include <stdlib.h>



int main(int argc, char** argv)
{
    logger_init(1, "asm.log");
    logger_set_level(INFO);

    char* filename = argv[1];
    FILE* text = open_file(filename);
    if (text == NULL)
    {
        printf("ERROR: bad file read.\n");
        exit(ERR_BAD_FILE);
    }

    long count = count_symbols(text);
    HANDLE_ERROR(count, ERR_BAD_PTR, "ERROR: pointer outside file.\n");

    char* buffer = (char*) calloc(count, sizeof(char));
    int ret = fill_buffer(text, buffer, sizeof(char), count);
    HANDLE_ERROR(ret, ERR_BAD_READ, "ERROR: file read error.\n");

    fclose(text);

    int number_strings = count_strings(buffer, count);
    struct string_t* strings = get_strings(buffer, count, number_strings);

    int number_labels = count_labels(strings, number_strings);
    label_t* labels = (label_t*) calloc(number_labels, sizeof(label_t));


    int fill_labels = 0; 
    compile(strings, number_strings, labels, &number_labels, fill_labels); // the first compile
    fill_labels = 1;
    compile(strings, number_strings, labels, &number_labels, fill_labels); // the second compile

    logger_finalize(file);

    return 0;
}
