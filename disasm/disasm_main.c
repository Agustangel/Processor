#include <stdio.h>
#include <stdlib.h>
#include <logger.h>
#include <onegin.h>
#include "cpu.h"



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
    int real_count_labels = 0;

    int found_labels = 0;
    decompile(code, count, pos_labels, &real_count_labels, found_labels);
    found_labels = 1;
    decompile(code, count, pos_labels, &real_count_labels, found_labels);

    logger_finalize(file);

    return 0;
}
