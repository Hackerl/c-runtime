#include "z_syscall.h"
#include "do_syscall.h"
#include <syscall.h>

#ifdef __aarch64__
#include <fcntl.h>
#endif

Z_RESULT_DECLARE(wrapper, long int) check_error(long int rc) {
    Z_RESULT(wrapper) r = {rc, 0};

    if (rc < 0 && rc > -4096) {
        r.v = -1;
        r.e = (int)-rc;
    }

    return r;
}

#define SYSCALL(name, ...) check_error(do_syscall(SYS_##name, __VA_ARGS__))

#define DEFINE_SYSCALL1(name, ret, t1, a1)                                          \
Z_RESULT(name) z_##name(t1 a1) {                                                    \
    Z_RESULT(wrapper) r = SYSCALL(name, a1);                                        \
    return (Z_RESULT(name)) {(ret)r.v, r.e};                                        \
}

#define DEFINE_SYSCALL2(name, ret, t1, a1, t2, a2)                                  \
Z_RESULT(name) z_##name(t1 a1, t2 a2) {                                             \
    Z_RESULT(wrapper) r = SYSCALL(name, a1, a2);                                    \
    return (Z_RESULT(name)) {(ret)r.v, r.e};                                        \
}

#define DEFINE_SYSCALL3(name, ret, t1, a1, t2, a2, t3, a3)                          \
Z_RESULT(name) z_##name(t1 a1, t2 a2, t3 a3) {                                      \
    Z_RESULT(wrapper) r = SYSCALL(name, a1, a2, a3);                                \
    return (Z_RESULT(name)) {(ret)r.v, r.e};                                        \
}

#define DEFINE_SYSCALL4(name, ret, t1, a1, t2, a2, t3, a3, t4, a4)                  \
Z_RESULT(name) z_##name(t1 a1, t2 a2, t3 a3, t4 a4) {                               \
    Z_RESULT(wrapper) r = SYSCALL(name, a1, a2, a3, a4);                            \
    return (Z_RESULT(name)) {(ret)r.v, r.e};                                        \
}

#define DEFINE_SYSCALL6(name, ret, t1, a1, t2, a2, t3, a3, t4, a4, t5, a5, t6, a6)  \
Z_RESULT(name) z_##name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, t6 a6) {                 \
    Z_RESULT(wrapper) r = SYSCALL(name, a1, a2, a3, a4, a5, a6);                    \
    return (Z_RESULT(name)) {(ret)r.v, r.e};                                        \
}

DEFINE_SYSCALL1(close, int, int, fd)
DEFINE_SYSCALL4(openat, int, int, dirfd, const char *, pathname, int, flags, mode_t, mode)
DEFINE_SYSCALL3(lseek, int, int, fd, off_t, offset, int, whence)
DEFINE_SYSCALL3(read, ssize_t, int, fd, void *, buf, size_t, count)
DEFINE_SYSCALL3(write, ssize_t, int, fd, const void *, buf, size_t, count)

DEFINE_SYSCALL2(munmap, int, void *, addr, size_t, length)
DEFINE_SYSCALL3(mprotect, int, void *, addr, size_t, length, int, prot)

DEFINE_SYSCALL6(futex, int, int *, uaddr, int, op, int, val, const struct timespec *, timeout, int *, uaddr2, int, val3)

void z_exit(int status) {
    SYSCALL(exit, status);
}

#ifdef __aarch64__
Z_RESULT(open) z_open(const char *pathname, int flags, mode_t mode) {
    Z_RESULT(wrapper) r = SYSCALL(openat, AT_FDCWD, pathname, flags, mode);
    return (Z_RESULT(open)) {(ret)r.v, r.errno};
}
#else
DEFINE_SYSCALL3(open, int, const char *, pathname, int, flags, mode_t, mode)
#endif

#if __i386__ || __arm__
DEFINE_SYSCALL2(stat64, int, const char *, pathname, struct stat64 *, buf)
#elif __x86_64__
DEFINE_SYSCALL2(stat, int, const char *, pathname, struct stat *, buf)
#elif __aarch64__
Z_RESULT(stat) z_stat(const char *pathname, struct stat *buf) {
    Z_RESULT(wrapper) r = SYSCALL(newfstatat, AT_FDCWD, pathname, buf, 0);
    return (Z_RESULT(open)) {(ret)r.v, r.errno};
}
#endif

#if __i386__ || __arm__
Z_RESULT(mmap) z_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    offset = (unsigned long long)offset >> 12;
    Z_RESULT(wrapper) r = SYSCALL(mmap2, addr, length, prot, flags, fd, offset);
    return (Z_RESULT(mmap)) {(ret)r.v, r.errno};
}
#else
DEFINE_SYSCALL6(mmap, void *, void *, addr, size_t, length, int, prot, int, flags, int, fd, off_t, offset)
#endif
