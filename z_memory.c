#include "z_memory.h"
#include "z_syscall.h"
#include "z_std.h"
#include <sys/mman.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x1000
#endif

#define CRT_PTR_USER(ptr)       ((unsigned long *)ptr + 2)
#define CRT_SIZE_USER(ptr)      (*((unsigned long *)ptr - 2))
#define CRT_SIZE_ALLOC(ptr)     (*((unsigned long *)ptr - 1))
#define CRT_SIZE_HDR            (2 * sizeof(unsigned long))

void z_free(void *ptr) {
    if (!ptr)
        return;

    z_munmap((char *)(ptr) - CRT_SIZE_HDR, CRT_SIZE_ALLOC(ptr));
}

void *z_realloc(void *ptr, size_t size){
    if (!size)
        return NULL;

    if (ptr && size < CRT_SIZE_ALLOC(ptr) - CRT_SIZE_HDR) {
        CRT_SIZE_USER(ptr) = size;
        return ptr;
    }

    size_t alloc_size = size + CRT_SIZE_HDR;

    if (alloc_size % PAGE_SIZE)
        alloc_size = ((alloc_size / PAGE_SIZE) + 1) * PAGE_SIZE;

    void *memory = z_mmap(
            NULL,
            alloc_size,
            PROT_READ | PROT_WRITE,
            MAP_ANONYMOUS | MAP_PRIVATE,
            -1,
            0);

    if (memory == MAP_FAILED) {
        return NULL;
    }

    memory = CRT_PTR_USER(memory);

    CRT_SIZE_USER(memory) = size;
    CRT_SIZE_ALLOC(memory) = alloc_size;

    if (ptr && CRT_SIZE_USER(ptr))
        z_memcpy(memory, ptr, CRT_SIZE_USER(ptr));

    if (ptr)
        z_free(ptr);

    return memory;
}

void *z_malloc(size_t size) {
    return z_realloc(NULL, size);
}