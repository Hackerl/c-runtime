#include "z_syscall.h"
#include "do_syscall.h"
#include <syscall.h>

void z_exit(int status) {
    do_syscall(SYS_exit, status);
}

ssize_t z_write(int fd, const void *buf, size_t count) {
    return (ssize_t)do_syscall(SYS_write, fd, buf, count);
}

void *z_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    return (void *)do_syscall(SYS_mmap, addr, length, prot, flags, fd, offset);
}

int z_munmap(void *addr, size_t length) {
    return (int)do_syscall(SYS_munmap, addr, length);
}