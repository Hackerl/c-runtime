#ifndef C_RUNTIME_Z_SYSCALL_H
#define C_RUNTIME_Z_SYSCALL_H

#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>

#define z_errno *z_errno_location()

int *z_errno_location();

void z_exit(int status);

int z_open(const char *pathname, int flags, mode_t mode);
int z_close(int fd);
int z_lseek(int fd, off_t offset, int whence);
ssize_t z_read(int fd, void *buf, size_t count);
ssize_t z_write(int fd, const void *buf, size_t count);

void *z_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int z_munmap(void *addr, size_t length);
int z_mprotect(void *addr, size_t length, int prot);

#if __i386__ || __arm__
int z_stat64(const char *pathname, struct stat64 *buf);
#elif __x86_64__ || __aarch64__
int z_stat(const char *pathname, struct stat *buf);
#endif

#endif //C_RUNTIME_Z_SYSCALL_H
