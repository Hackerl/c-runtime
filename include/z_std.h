#ifndef C_RUNTIME_Z_STD_H
#define C_RUNTIME_Z_STD_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t z_strlen(const char * str);
int z_strncmp(const char * s1, const char * s2, size_t n);
int z_strcmp(const char * s1, const char * s2);

void *z_memcpy(void *dst, const void *src, size_t n);
int z_memcmp(const void *s1, const void *s2, size_t n);
void *z_memset(void *s, unsigned char c, size_t n);
void *z_memmem(void *haystack, size_t n, void *needle, size_t m);

int z_isspace(int c);
int z_isdigit(int c);
int z_isalpha(int c);
int z_isupper(int c);

long int z_strtol(const char *str, char **endptr, int base);
unsigned long int z_strtoul(const char *str, char **endptr, int base);

#ifdef __cplusplus
}
#endif

#endif //C_RUNTIME_Z_STD_H
