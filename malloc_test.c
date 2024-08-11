#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    void *ptr = malloc(12345);

    free(ptr);
}