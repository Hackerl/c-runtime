#include <z_memory.h>
#include <z_syscall.h>
#include <z_std.h>
#include <z_sync.h>
#include <sys/mman.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x1000
#endif

#define CRT_PTR_RAW(ptr)        ((unsigned long *)(ptr) - 2)
#define CRT_PTR_USER(ptr)       ((unsigned long *)(ptr) + 2)
#define CRT_SIZE_USER(ptr)      (*((unsigned long *)(ptr) - 2))
#define CRT_SIZE_ALLOC(ptr)     (*((unsigned long *)(ptr) - 1))
#define CRT_SIZE_HDR            (2 * sizeof(unsigned long))

#define HEAP_CACHE_COUNT        100
#define HEAP_CACHE_LOWER        0x1000
#define HEAP_CACHE_UPPER        0x10000
#define HEAP_CACHE_RETRY        5

static void *cache_buffer[HEAP_CACHE_COUNT] = {};
static z_circular_buffer_state_t cache_state[HEAP_CACHE_COUNT] = {};

static z_circular_buffer_t cache = {
        0,
        0,
        sizeof(void *),
        HEAP_CACHE_COUNT,
        cache_buffer,
        cache_state
};

void z_free(void *ptr) {
    if (!ptr)
        return;

    if (CRT_SIZE_ALLOC(ptr) >= HEAP_CACHE_LOWER && CRT_SIZE_ALLOC(ptr) <= HEAP_CACHE_UPPER && z_circular_buffer_enqueue(&cache, &ptr))
        return;

    z_munmap(CRT_PTR_RAW(ptr), CRT_SIZE_ALLOC(ptr));
}

void *z_realloc(void *ptr, size_t size){
    if (ptr && size <= CRT_SIZE_ALLOC(ptr) - CRT_SIZE_HDR) {
        CRT_SIZE_USER(ptr) = size;
        return ptr;
    }

    void *p = z_malloc(size);

    if (ptr) {
        z_memcpy(p, ptr, CRT_SIZE_USER(ptr) < size ? CRT_SIZE_USER(ptr) : size);
        z_free(ptr);
    }

    return p;
}

void *z_malloc(size_t size) {
    void *ptr = NULL;
    size_t length = (size + CRT_SIZE_HDR + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    if (length <= HEAP_CACHE_UPPER) {
        for (int i = 0; i < HEAP_CACHE_RETRY; i++, ptr = NULL) {
            if (!z_circular_buffer_dequeue(&cache, &ptr))
                break;

            if (CRT_SIZE_ALLOC(ptr) >= length) {
                CRT_SIZE_USER(ptr) = size;
                break;
            }

            if (!z_circular_buffer_enqueue(&cache, &ptr)) {
                z_free(ptr);
            }
        }
    }

    if (!ptr) {
        void *memory = Z_RESULT_V(z_mmap(
                NULL,
                length,
                PROT_READ | PROT_WRITE,
                MAP_ANONYMOUS | MAP_PRIVATE,
                -1,
                0));

        if (memory == MAP_FAILED) {
            return NULL;
        }

        ptr = CRT_PTR_USER(memory);

        CRT_SIZE_USER(ptr) = size;
        CRT_SIZE_ALLOC(ptr) = length;
    }

    return ptr;
}

void *z_calloc(size_t num, size_t size) {
    void *ptr = z_malloc(num * size);

    if (ptr)
        z_memset(ptr, 0, num * size);

    return ptr;
}