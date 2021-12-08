#ifndef C_RUNTIME_Z_SYSCALL_H
#define C_RUNTIME_Z_SYSCALL_H

#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

#define Z_RESULT_V(r) r.v
#define Z_RESULT(name) struct z_result_##name

#define Z_RESULT_DECLARE(name, t)           \
Z_RESULT(name) {                            \
    t v;                                    \
    int e;                                  \
};                                          \
                                            \
Z_RESULT(name)                              \

long int z_syscall(long int sys_no, ...);

void z_exit(int status);
void z_exit_group(int status);

Z_RESULT_DECLARE(kill, int) z_kill(pid_t pid, int sig);

Z_RESULT_DECLARE(open, int) z_open(const char *pathname, int flags, mode_t mode);
Z_RESULT_DECLARE(openat, int) z_openat(int dirfd, const char *pathname, int flags, mode_t mode);
Z_RESULT_DECLARE(close, int) z_close(int fd);
Z_RESULT_DECLARE(lseek, int) z_lseek(int fd, off_t offset, int whence);
Z_RESULT_DECLARE(read, ssize_t) z_read(int fd, void *buf, size_t count);
Z_RESULT_DECLARE(write, ssize_t) z_write(int fd, const void *buf, size_t count);

Z_RESULT_DECLARE(mmap, void *) z_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
Z_RESULT_DECLARE(munmap, int) z_munmap(void *addr, size_t length);
Z_RESULT_DECLARE(mprotect, int) z_mprotect(void *addr, size_t length, int prot);

Z_RESULT_DECLARE(futex, int) z_futex(int *uaddr, int op, int val, const struct timespec *timeout, int *uaddr2, int val3);

#if __i386__ || __x86_64__
Z_RESULT_DECLARE(arch_prctl, int) z_arch_prctl(int code, unsigned long address);
#endif

#if __i386__ || __arm__
Z_RESULT_DECLARE(stat64, int) z_stat64(const char *pathname, struct stat64 *buf);
#elif __x86_64__ || __aarch64__
Z_RESULT_DECLARE(stat, int) z_stat(const char *pathname, struct stat *buf);
#endif

#ifdef __cplusplus
}
#endif

#endif //C_RUNTIME_Z_SYSCALL_H
