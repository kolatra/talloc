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
        printf("free: %s\n", bool_to_string(current->free));
        printf("magic: %d\n", current->magic);
        printf("--------------------------------------\n");
        current = current->next;
    }
}

struct block_meta *find_free_block(struct block_meta *last, size_t size) {
    struct block_meta *current = last;

    while (current && !(current->free && current->size >= size)) {
        current = current->next;
    }

    return current;
}

struct block_meta *request_memory(size_t size) {
    // The other option for requesting memory is mmap,
    // useful if allocating shared memory
    struct block_meta *block = sbrk(0);
    void *req_memory = sbrk(META_SIZE + size);
    fail_if(block != req_memory, "something else moved brk");

    if (req_memory != (void *)-1) {
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
    size = size + META_SIZE;
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
        struct block_meta *free_block = find_free_block(global_base, size);

        if (free_block) {
            fail_if(!free_block->free, "[!] Somehow got a non-free block");

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

    printf("[~] Block alloc'd at %p\n", ret);
    // Move to the next "element" so that we can start
    // using the space after the metadata
    return ret + 1;
}

void my_free(struct block_meta *ptr) {
    if (ptr->free) {
        printf("[!] Block starting at %p was already freed.\n", ptr);
        return;
    }

    ptr->free = true;
    ptr->magic = 0x25;
    printf("[~] Freed %p\n", ptr);
}

int init_heap(struct heap_meta *heap) {
    struct block_meta *mem = my_malloc(1);
    if (!mem) return -1;

    heap->start = mem;
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
        char *test = (char *) my_malloc(50);

        printf("[~] %d addr: %p\n", i, test);
        fail_if(!test, "Could not get a block");

        // Assign 26 bytes to something
        for (char j = 0x41; j <= 0x5a; j++) {
            *test++ = j;
        }

        // move back to the start
        test -= 26;

        printf("[~] %s\n", test);

        allocations[i] = (struct block_meta *)test - 1;
    }

    struct block_meta *empty = my_malloc(0);
    fail_if(empty != NULL, "0 malloc did not return null");

    // print the blocks after we make them
    printf("The blocks before freeing\n");
    // print_list();

    // free them
    for (int i = 0; i < COUNT; i++) {
        my_free(allocations[i]);
    }
    my_free(heap.start);

    // print again
    printf("The blocks after\n");
    // print_list();

    return 0;
}