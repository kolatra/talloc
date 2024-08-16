#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>

typedef struct {
    FILE* fp;
    size_t size;
} FileHandler;

void fail_if(bool flag, const char *message, ...);
const char *bool_to_string(bool flag);
int count_lines(char *input);
int count_lines_in_file(FILE *fp);
int file_size(FILE *fp);
FileHandler *open_file(char *path, const char *mode);
