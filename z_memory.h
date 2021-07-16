#ifndef C_RUNTIME_Z_MEMORY_H
#define C_RUNTIME_Z_MEMORY_H

#include <stddef.h>

void *z_malloc(size_t size);
void *z_realloc(void *ptr, size_t size);
void z_free(void *ptr);

#endif //C_RUNTIME_Z_MEMORY_H
