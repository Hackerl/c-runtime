#include <z_memory.h>
#include <z_syscall.h>
#include <z_std.h>
#include <sys/mman.h>
#include <stdint.h>

#ifndef DISABLE_CACHE
#include <z_sync.h>
#endif

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x1000
#endif

#define CRT_SIZE_HDR            (2 * sizeof(size_t))
#define CRT_PTR_RAW(ptr)        ((void *)((uintptr_t)ptr - CRT_SIZE_HDR))
#define CRT_PTR_USER(ptr)       ((void *)((uintptr_t)ptr + CRT_SIZE_HDR))
#define CRT_SIZE_USER(ptr)      (*((size_t *)ptr - 2))
#define CRT_SIZE_ALLOC(ptr)     (*((size_t *)ptr - 1))

#ifndef DISABLE_CACHE

#define HEAP_CACHE_COUNT        100
#define HEAP_CACHE_LOWER        0x1000
#define HEAP_CACHE_UPPER        0x10000
#define HEAP_CACHE_RETRY        5
#define HEAP_CACHE_EXPIRED      5

typedef struct {
    int miss;
    void *ptr;
} cache_record_t;

static cache_record_t cache_records[HEAP_CACHE_COUNT] = {};
static z_circular_buffer_state_t cache_states[HEAP_CACHE_COUNT] = {};

static z_circular_buffer_t cache = {
        0,
        0,
        sizeof(cache_record_t),
        HEAP_CACHE_COUNT,
        cache_records,
        cache_states
};

#endif

void z_free(void *ptr) {
    if (!ptr)
        return;

#ifndef DISABLE_CACHE
    if (CRT_SIZE_ALLOC(ptr) >= HEAP_CACHE_LOWER && CRT_SIZE_ALLOC(ptr) <= HEAP_CACHE_UPPER) {
        cache_record_t record = {0, ptr};

        if (z_circular_buffer_enqueue(&cache, &record)) {
            return;
        }
    }
#endif

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
    size_t length = (size + CRT_SIZE_HDR + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

#ifndef DISABLE_CACHE
    if (length <= HEAP_CACHE_UPPER) {
        cache_record_t record = {};

        for (int i = 0; i < HEAP_CACHE_RETRY; i++) {
            if (!z_circular_buffer_dequeue(&cache, &record))
                break;

            if (CRT_SIZE_ALLOC(record.ptr) >= length) {
                CRT_SIZE_USER(record.ptr) = size;
                return record.ptr;
            }

            if (record.miss++ > HEAP_CACHE_EXPIRED || !z_circular_buffer_enqueue(&cache, &record)) {
                z_munmap(CRT_PTR_RAW(record.ptr), CRT_SIZE_ALLOC(record.ptr));
            }
        }
    }
#endif

    void *ptr = Z_RESULT_V(z_mmap(
            NULL,
            length,
            PROT_READ | PROT_WRITE,
            MAP_ANONYMOUS | MAP_PRIVATE,
            -1,
            0));

    if (ptr == MAP_FAILED) {
        return NULL;
    }

    ptr = CRT_PTR_USER(ptr);

    CRT_SIZE_ALLOC(ptr) = length;
    CRT_SIZE_USER(ptr) = size;

    return ptr;
}

void *z_calloc(size_t num, size_t size) {
    void *ptr = z_malloc(num * size);

    if (ptr)
        z_memset(ptr, 0, num * size);

    return ptr;
}