# sd

writing a custom malloc in c:
    - request a region of memory from the kernel, a page maybe?
    - split it into chunks and hold the starting chunk pointer in a struct

    - what actually happens when we request heap memory?
    - we first check if we have enough space in the data segment for the requested space
    - if not then we use `allocate_memory_block(size)` to request a new block of memory to use
    - we get the pointer to the current break point with `sbrk
    - we use `sbrk` again with the size of our chunk to request additional memory

note: the break point starts at the end of the data segment
