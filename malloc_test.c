#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "malloc.h"

#define BRUTEFORCE

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: ./malloc <path>\n");
        return 1;
    }

#ifdef BRUTEFORCE

    int count = atoi(argv[1]);
    #define TYPE int

    for (int i = 0; i < sizeof(TYPE) * count; i++) {
        TYPE *ptr = (TYPE *)my_malloc(sizeof(TYPE));
        *ptr = i;
        // printf("ADDR: %p VALUE: %d\n", ptr, *ptr);
        ptr++;
    }

#else // BRUTEFORCE

#include "util.h"
    char *path = argv[1];    
    FileHandler* handler = open_file(path, "r");
    BLOCK_PTR *blocks[5];
    for (int i = 0; i < 5; i++) {
        int lines = count_lines_in_file(handler->fp);
        char *buf = (char *)malloc(handler->size - lines);
        blocks[i] = (BLOCK_PTR *)buf;

        int j = 0;
        char c = 0;
        while ((c = fgetc(handler->fp)) != EOF) {
            if (c == '\n') continue;
            buf[j] = c;
            j++;
        }

        printf("[~] Buffer %d allocated.\n", i + 1);
    }

    for (int i = 0; i < 5; i++) {
        free(blocks[i]);
    }
#endif // BRUTEFORCE
}