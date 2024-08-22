#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    #define COUNT 25000000
    #define TYPE int

    TYPE *ptr = (TYPE *)malloc(sizeof(TYPE) * COUNT);

    for (int i = 0; i < COUNT; i++) {
        *ptr = i;
        printf("ADDR: %p VALUE: %d\n", ptr, *ptr);
        ptr++;
    }
}