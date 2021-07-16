#include "z_syscall.h"
#include "do_syscall.h"
#include <syscall.h>

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

DEFINE_SYSCALL2(open, int, const char *, filename, int, flags)
DEFINE_SYSCALL1(close, int, int, fd)
DEFINE_SYSCALL3(lseek, int, int, fd, off_t, off, int, whence)
DEFINE_SYSCALL3(read, ssize_t, int, fd, void *, buf, size_t, count)
DEFINE_SYSCALL3(write, ssize_t, int, fd, const void *, buf, size_t, count)

DEFINE_SYSCALL2(munmap, int, void *, addr, size_t, length)
DEFINE_SYSCALL3(mprotect, int, void *, addr, size_t, length, int, prot)

void *z_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
#ifdef __i386__
    /* i386 has old_mmap and mmap2, old_map is a legacy single arg
	 * function, use mmap2 but it needs offset in page units. */
	offset = (unsigned long long)offset >> 12;
	return (void *)SYSCALL(mmap2, addr, length, prot, flags, fd, offset);
#else
    return (void *)SYSCALL(mmap, addr, length, prot, flags, fd, offset);
#endif
}