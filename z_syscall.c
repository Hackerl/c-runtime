#include "z_syscall.h"
#include "do_syscall.h"
#include <syscall.h>

#ifdef __aarch64__
#include <fcntl.h>
#endif

static int errno;

int *z_errno_location() {
    return &errno;
}

static long int check_error(long int rc) {
    if (rc < 0 && rc > -4096) {
        errno = (int)-rc;
        rc = -1;
    }

    return rc;
}

#define SYSCALL(name, ...) check_error(do_syscall(SYS_##name, __VA_ARGS__))

#define DEFINE_SYSCALL1(name, ret, t1, a1)                                  \
ret z_##name(t1 a1) {                                                       \
    return (ret)SYSCALL(name, a1);                                          \
}

#define DEFINE_SYSCALL2(name, ret, t1, a1, t2, a2)                          \
ret z_##name(t1 a1, t2 a2) {                                                \
    return (ret)SYSCALL(name, a1, a2);                                      \
}

#define DEFINE_SYSCALL3(name, ret, t1, a1, t2, a2, t3, a3)                  \
ret z_##name(t1 a1, t2 a2, t3 a3) {                                         \
    return (ret)SYSCALL(name, a1, a2, a3);                                  \
}

DEFINE_SYSCALL1(exit, void, int, status)

DEFINE_SYSCALL1(close, int, int, fd)
DEFINE_SYSCALL3(lseek, int, int, fd, off_t, offset, int, whence)
DEFINE_SYSCALL3(read, ssize_t, int, fd, void *, buf, size_t, count)
DEFINE_SYSCALL3(write, ssize_t, int, fd, const void *, buf, size_t, count)

DEFINE_SYSCALL2(munmap, int, void *, addr, size_t, length)
DEFINE_SYSCALL3(mprotect, int, void *, addr, size_t, length, int, prot)

int z_open(const char *pathname, int flags, mode_t mode) {
#ifdef __aarch64__
    return (int)SYSCALL(openat, AT_FDCWD, pathname, flags, mode);
#else
    return (int)SYSCALL(open, pathname, flags, mode);
#endif
}

int z_stat(const char *pathname, struct stat *buf) {
#ifdef __aarch64__
    return (int)SYSCALL(newfstatat, AT_FDCWD, pathname, buf, 0);
#else
    return (int)SYSCALL(stat, pathname, buf);
#endif
}

void *z_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
#if __i386__ || __arm__
    offset = (unsigned long long)offset >> 12;
    return (void *)SYSCALL(mmap2, addr, length, prot, flags, fd, offset);
#else
    return (void *)SYSCALL(mmap, addr, length, prot, flags, fd, offset);
#endif
}