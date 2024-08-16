#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    #define COUNT 25
    #define TYPE int

    TYPE *ptr = (TYPE *)malloc(sizeof(TYPE) * COUNT);

    for (int i = 0; i < COUNT; i++) {
        *ptr = i;
        printf("ADDR: %p VALUE: %d\n", ptr, *ptr);
        ptr++;
    }
    printf("Found the null byte.\n");
    printf("haha just kidding there is no null byte, track the size of the allocation manually\n");
}