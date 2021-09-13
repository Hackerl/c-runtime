#ifndef C_RUNTIME_Z_MEMORY_H
#define C_RUNTIME_Z_MEMORY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *z_malloc(size_t size);
void *z_realloc(void *ptr, size_t size);
void *z_calloc(size_t num, size_t size);
void z_free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif //C_RUNTIME_Z_MEMORY_H
