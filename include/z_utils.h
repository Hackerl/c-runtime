#ifndef C_RUNTIME_Z_UTILS_H
#define C_RUNTIME_Z_UTILS_H

#include <stddef.h>

long z_get_file_size(const char *path);
int z_read_file(const char *path, char **buffer, size_t *length);

#endif //C_RUNTIME_Z_UTILS_H
