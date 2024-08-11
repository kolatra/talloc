#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

void fail_if(bool flag, char *message) {
    assert(!flag);

    // if (flag) {
    //     printf("%s\n", message);
    //     exit(-1);
    // }
}
