#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "util.h"

#define META_SIZE sizeof(struct block_meta)

struct block_meta *global_base = NULL;
// TODO make this a property of heap_meta most likely
struct block_meta *latest = NULL;

struct heap_meta {
    struct block_meta *start;
};

struct block_meta {
    size_t size;
    struct block_meta *addr;
    struct block_meta *next;
    bool free;
    int magic;
};

void print_list() {
    struct block_meta *current = global_base;

    while (current) {
        printf("size: %ld\n", current->size);
        printf("addr: %p\n", current->addr);
        printf("next: %p\n", current->next);
        printf("free: %s\n", current->free ? "true" : "false");
        printf("magic: %d\n", current->magic);
        printf("--------------------------------------\n");
        current = current->next;
    }
}

struct block_meta *find_free_block(struct block_meta *last, size_t size) {
    struct block_meta *current = last;

    while (current && !(current->free && current->size >= size)) {
        last = current;
        current = current->next;
    }

    return current;
}

struct block_meta *request_memory(size_t size) {
    struct block_meta *block = sbrk(0);
    void *req_memory = sbrk(META_SIZE + size);
    printf("[+] sbrk address: %p\n", block);
    fail_if(block != req_memory, "something else moved brk");
    // TIL that printf calls malloc and moves the break pointer

    if (req_memory != (void *)-1 && block == req_memory) {
        block->free = false;
        block->size = size;
        block->addr = req_memory;
        block->next = NULL;
        block->magic = 0x12345678;
        return block;
    }

    return NULL;
}

void *my_malloc(size_t size) {
    if (size <= 0) {
        return NULL;
    }

    // create enough room for the block metadata
    size = size + 28;
    printf("Size: %ld\n", size);

    struct block_meta *ret = NULL;

    if (!global_base) {
        struct block_meta *first_block = request_memory(size);
        fail_if(first_block == NULL, "could not request block");

        global_base = first_block;
        latest = first_block;
        printf("[~] Setting global base to %p\n", first_block);

        ret = first_block;
    } else {
        // check to see if we can reuse any existing space
        // if we can, then we do
        // if we can't then we request new space and use that
        struct block_meta *free_block = find_free_block(global_base, size);

        if (free_block) {
            fail_if(!free_block->free, "._.");

            printf(
                "[~] Found a previously free block at %p with %ld length\n",
                free_block,
                free_block->size
            );

            free_block->free = false;
            ret = free_block;
        } else {
            struct block_meta *new_space = request_memory(size);
            fail_if(!new_space, "could not request block");

            latest->next = new_space;
            latest = latest->next;

            ret = new_space;
        }
    }

    return ret + 1;
}

void my_free(struct block_meta *ptr) {
    if (ptr->free) {
        printf("[!] Block starting at %p was already freed.\n", ptr);
        return;
    }

    ptr->free = true;
    ptr->magic = 0x87654321;
    printf("[!] Freed %p\n", ptr);
}

int init_heap(struct heap_meta *heap) {
    struct block_meta *mem = my_malloc(1);
    if (!mem) return -1;

    // The chunk metadata is one element behind in memory, store that
    heap->start = mem - 1;
    printf("[~] size: %ld\n", mem->size);

    return 0;
}

int main() {
    struct heap_meta heap = {0};
    fail_if(init_heap(&heap) == -1, "Could not initialize heap");
    printf("[~] Heap initialized\n");

    // Allocate multiple times to test the linked list
    #define COUNT 15
    struct block_meta *allocations[COUNT];
    for (int i = 0; i < COUNT; i++) {
        size_t *test = (size_t *) my_malloc(100);
        // save a copy of the start so we can free after
        struct block_meta* start = (struct block_meta *)test;

        printf("[~] %d addr: %p\n", i, test);
        fail_if(!test, "Could not get a block");

        // Assign 25 bytes to something
        for (size_t i = 0; i < 25; i++) {
            *test = i;
            // printf("[#] ADDRESS: %p DEREF: %ld\n", test, *test);
            test++;
        }

        allocations[i] = start - 1;
    }

    struct block_meta *empty = my_malloc(0);
    fail_if(empty != NULL, "0 malloc did not return null");

    // print the blocks after we make them
    printf("The blocks before freeing\n");
    print_list();

    // free them
    for (int i = 0; i < COUNT; i++) {
        my_free(allocations[i]);
    }
    my_free(heap.start);

    // print again
    printf("The blocks after\n");
    print_list();

    return 0;
}