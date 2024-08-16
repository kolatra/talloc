#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "util.h"

#define META_SIZE sizeof(struct block_meta)

// Always points to the first block
struct block_meta *blocks_list = NULL;
struct block_meta *latest = NULL;
struct block_meta *free_list = NULL;

struct block_meta {
    size_t size;
    struct block_meta *addr;
    struct block_meta *next;
    struct block_meta *prev;
    bool free;
    int magic;
};

void print_list() {
    struct block_meta *current = blocks_list;

    while (current) {
        printf("[~] Block size: %ld\n", current->size);
        printf("[~] Block addr: %p\n", current->addr);
        printf("[~] Block next: %p\n", current->next);
        printf("[~] Block prev: %p\n", current->prev);
        printf("[~] Block free: %s\n", bool_to_string(current->free));
        printf("[~] Block magic: %d\n", current->magic);
        printf("--------------------------------------\n");
        current = current->next;
    }
}

struct block_meta *find_free_block(size_t size) {
    struct block_meta *current = free_list;

    while (current && !(current->free && current->size >= size)) {
        current = current->next;
    }

    return current;
}

struct block_meta *request_memory(size_t size) {
    // The other option for requesting memory is mmap,
    // useful if allocating shared memory
    struct block_meta *block = sbrk(size);
    // initialization for the first block
    block->prev = NULL;

    if (block != (void *)-1) {
        block->free = false;
        block->size = size;
        block->addr = block + 1;
        block->next = NULL;
        block->magic = 0x49;
        return block;
    }

    return NULL;
}

void *my_malloc(size_t size) {
    if (size <= 0) {
        return NULL;
    }

    // create enough room for the block metadata
    size += META_SIZE;

    struct block_meta *ret;

    if (!blocks_list) {
        struct block_meta *first_block = request_memory(size);
        fail_if(first_block == NULL, "[!] could not request first block");

        blocks_list = first_block;
        latest = first_block;
        printf("[~] Setting global base to %p\n", first_block);

        ret = first_block;
    } else {
        struct block_meta *free_block = find_free_block(size);

        if (free_block) {
            fail_if(!free_block->free, "[!] Somehow got a non-free block");

            // Optimization:
            // this current returns very large blocks for much smaller allocations 
            // is there a way to prefer smaller blocks?
            // or reuse the old block and give the memory back to the OS.
            //
            // if a freed chunk is larger than 512 mb, split into 512 mb
            // chunks and store them in a free list
            printf(
                "[~] Found a previously free block at %p with %ld length\n",
                free_block,
                free_block->size
            );

            free_block->free = false;
            ret = free_block;
        } else {
            struct block_meta *new_space = request_memory(size);
            fail_if(!new_space, "[!] could not request a block");

            new_space->prev = latest;
            latest->next = new_space;
            latest = latest->next;

            ret = new_space;
        }
    }

    printf("[~] Block created at %p with %zu bytes\n", ret, ret->size);
    return ret->addr;
}

void update_frees(struct block_meta *exiled_entry) {
    if (exiled_entry == blocks_list) 
        blocks_list = blocks_list->next;

    if (exiled_entry->next)
        exiled_entry->next->prev = exiled_entry->prev;

    if (exiled_entry->prev)
        exiled_entry->prev->next = exiled_entry->next;

    exiled_entry->next = NULL;

    if (free_list) {
        struct block_meta *current = free_list;
        // This is infinite looping because I forgot to
        // remove the entries from here that get reused
        // tomorrow's fix :)
        while (current->next) {
            current = current->next;
        }
        current->next = exiled_entry;
        exiled_entry->prev = current;
    } else {
        free_list = exiled_entry;
    }
}

void my_free(struct block_meta *ptr) {
    if (ptr->free) {
        printf("[!] Block starting at %p was already freed.\n", ptr);
        return;
    }
    
    size_t len = ptr->size;
    char *bytes = (char *)ptr->addr;
    for (int i = 0; i < len - META_SIZE; i++) {
        // omg i'm overwriting my own heap metadata because i'm using the full len including the metadata and not just the block size
        bytes[i] = 0;
    }

    ptr->free = true;
    ptr->magic = 0x45;
    
    update_frees(ptr);

    printf("[~] Freed %p\n", ptr);
}

int main() {
    FileHandler* handler = open_file("./numbers.txt", "r");
    struct block_meta *ptrs[5];
    for (int i = 0; i < 5; i++) {
        int lines = count_lines_in_file(handler->fp);
        char *buf = (char *)my_malloc(handler->size - lines);
        ptrs[i] = (struct block_meta *)buf - 1;

        for (int i = 0; i < handler->size; i++) {
            char c = fgetc(handler->fp);
            if (c == EOF) break;
            if (c == '\n') {
                i--;
                continue;   
            }
            buf[i] = c;
        }

        printf("Buffer allocated.\n");
    }

    for (int i = 0; i < 5; i++) {
        my_free(ptrs[i]);
    }
#define DEBUG
#ifdef DEBUG
    // Allocate multiple times to test the linked list
    #define COUNT 15
    struct block_meta *allocations[COUNT];
    for (int i = 0; i < COUNT; i++) {
        char *test = (char *) my_malloc(50);

        printf("[~] %d addr: %p\n", i, test);
        fail_if(!test, "Could not get a block");

        allocations[i] = (struct block_meta *)test - 1;

        // Assign 26 bytes to something
        for (char j = 0x41; j <= 0x5a; j++) {
            *test++ = j;
        }
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

    // print again
    printf("The blocks after\n");
    // print_list();
#endif // DEBUG

    return 0;
}
