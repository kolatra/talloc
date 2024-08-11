#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    // malloc(12345);

    void *brk = sbrk(0);
    printf("\n");
    void *brk1 = sbrk(0);

    printf("%p %p\n", brk, brk1);
}