#pragma once

#include <stdio.h>
#include <stdbool.h>

#define malloc(x) my_malloc(x)
#define free(x) my_free(x)

void *my_malloc(size_t size);
void my_free(void *p);

#define META_SIZE sizeof(struct block_meta)
#define BLOCK_PTR struct block_meta

BLOCK_PTR {
    size_t size;
    BLOCK_PTR *addr;
    BLOCK_PTR *next;
    BLOCK_PTR *prev;
    bool free;
    int magic;
};
