#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    int *ptr = (int *)malloc(sizeof(int) * 25);

    for (int i = 0; i < 25; i++) {
        *ptr = i;
        printf("ADDR: %p VALUE: %d\n", ptr, *ptr++);
    }
    printf("Found the null byte.\n");
    printf("haha just kidding there is no null byte, track the size of the allocation manually\n");
}