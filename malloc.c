#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "util.h"
#include "malloc.h"

BLOCK_PTR *blocks_list = NULL;
BLOCK_PTR *latest = NULL;
BLOCK_PTR *free_list = NULL;

void remove_from_list(BLOCK_PTR *block) {
    if (block->next && block->prev)
        block->next->prev = block->prev;
    else if (block->next)
        block->next->prev = NULL;
    else if (block->prev)
        block->prev->next = NULL;
}

void print_list() {
    BLOCK_PTR *current = blocks_list;

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

BLOCK_PTR *find_free_block(size_t size) {
    BLOCK_PTR *current = free_list;

    while (current && !(current->free && current->size >= size)) {
        current = current->next;
    }

    return current;
}

BLOCK_PTR *request_memory(size_t size) {
    // The other option for requesting memory is mmap,
    // useful if allocating shared memory
    BLOCK_PTR *block = sbrk(size + META_SIZE);

    if (block != (void *)-1) {
        block->free = false;
        block->size = size;
        block->addr = block + 1;
        block->next = NULL;
        block->prev = NULL;
        block->magic = 0x49;
        return block;
    }

    return NULL;
}

void *my_malloc(size_t size) {
    if (size <= 0) {
        return NULL;
    }

    BLOCK_PTR *ret;

    if (!blocks_list) {
        BLOCK_PTR *first_block = request_memory(size);
        fail_if(first_block == NULL, "[!] could not request first block");

        blocks_list = first_block;
        latest = first_block;
        printf("[~] Setting global base to %p\n", first_block);

        ret = first_block;
    } 
    
    else {
        BLOCK_PTR *free_block = find_free_block(size);

        if (free_block) {
            fail_if(!free_block->free, "[!] Somehow got a non-free block");

            // Optimization:
            // this current returns very large blocks for much smaller allocations 
            // is there a way to prefer smaller blocks?
            // or reuse the old block and give the memory back to the OS.
            //
            // if a freed chunk is larger than 512 mb, split into 512 mb
            // chunks and store them in a free list
            //
            // give the free chunk with only the requested size, 
            // then put a dummy free block in at the start of the unused memory
            printf(
                "[~] Found a previously free block at %p with %ld length\n",
                free_block,
                free_block->size
            );

            free_block->free = false;
            ret = free_block;

            remove_from_list(free_block);
        } 
        
        else {
            BLOCK_PTR *new_space = request_memory(size);
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

void add_to_free_list(BLOCK_PTR *exiled_entry) {
    if (exiled_entry == blocks_list && blocks_list->next) 
        blocks_list = blocks_list->next;

    remove_from_list(exiled_entry);

    exiled_entry->next = NULL;

    if (free_list) {
        BLOCK_PTR *current = free_list;
        while (current->next) {
            current = current->next;
        }
        current->next = exiled_entry;
        exiled_entry->prev = current;
    } else {
        free_list = exiled_entry;
    }
}

void my_free(void *p) {
    BLOCK_PTR *ptr = (BLOCK_PTR *)p - 1;

    if (ptr->free) {
        printf("[!] Block starting at %p was already freed.\n", ptr);
        return;
    }

    memset(ptr->addr, 0, ptr->size);

    ptr->free = true;
    ptr->magic = 0x45;
    
    add_to_free_list(ptr);

    printf("[~] Freed %p\n", ptr);
}
