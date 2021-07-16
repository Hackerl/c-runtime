#ifndef C_RUNTIME_Z_SYSCALL_H
#define C_RUNTIME_Z_SYSCALL_H

#include <stddef.h>
#include <sys/types.h>

void z_exit(int status);

ssize_t z_write(int fd, const void *buf, size_t count);

void *z_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int z_munmap(void *addr, size_t length);

#endif //C_RUNTIME_Z_SYSCALL_H
