#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

void fail_if(bool flag, char *message) {
    if (flag) {
        printf("%s\n", message);
        exit(-1);
    }
}

char *bool_to_string(bool flag) {
    return flag ? "true" : "false";
}
