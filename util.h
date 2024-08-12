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
