#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>

void fail_if(bool flag, char *message, ...) {
    va_list arg_p;
    va_start(arg_p, message);

    if (flag) {
        vprintf(message, arg_p);
        exit(-1);
    }
}

char *bool_to_string(bool flag) {
    return flag ? "true" : "false";
}

int count_lines(char* input) {
    int ret = 1;

    for (char *c = input; c; c++) {
        if (*c == '\n') {
            ret++;
        }
    }

    return ret;
}

int file_size(FILE* fp) {
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    return size;
}

typedef struct {
    FILE* fp;
    size_t size;
} FileHandler;

FileHandler* open_file(char* path, const char* mode) {
    FileHandler *handler = (FileHandler *)malloc(sizeof(FileHandler));

    FILE* fp = fopen(path, mode);
    fail_if(!fp, "Could not open file at %s.", path);

    handler->fp = fp;
    handler->size = file_size(fp);

    return handler;
}
