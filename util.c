#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

void fail_if(bool flag, const char *message, ...) {
    va_list arg_p;
    va_start(arg_p, message);

    if (flag) {
        vprintf(message, arg_p);
        exit(-1);
    }
}

const char *bool_to_string(bool flag) {
    return flag ? "true" : "false";
}

int count_lines(char *input) {
    int ret = 1;

    for (char *c = input; c; c++) {
        if (*c == '\n') {
            ret++;
        }
    }

    return ret;
}

int file_size(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    return size;
}

int count_lines_in_file(FILE *fp) {
    int ret = 0;

    int c;
    while ((c = fgetc(fp) != EOF)) {
        if (c == '\n') {
            ret++;
        }
    }

    fseek(fp, 0, SEEK_SET);

    return ret;
}

FileHandler* open_file(char* path, const char* mode) {
    FileHandler *handler = (FileHandler *)malloc(sizeof(FileHandler));

    FILE* fp = fopen(path, mode);
    fail_if(!fp, "Could not open file at %s.", path);

    handler->fp = fp;
    handler->size = file_size(fp);

    printf("[~] File: %s Size: %zu\n", path, handler->size);

    return handler;
}
