#include <z_syscall.h>
#include <syscall.h>

#ifdef __aarch64__
#include <fcntl.h>
#endif

#ifdef __i386__

__asm__ (
".section .text;"
".global z_syscall;"
".internal z_syscall;"
".type z_syscall, @function;"
"z_syscall:"
"    push %ebp;"
"    push %edi;"
"    push %esi;"
"    push %ebx;"
"    mov 44(%esp), %ebp;"
"    mov 40(%esp), %edi;"
"    mov 36(%esp), %esi;"
"    mov 32(%esp), %edx;"
"    mov 28(%esp), %ecx;"
"    mov 24(%esp), %ebx;"
"    mov 20(%esp), %eax;"
"    int $0x80;"
"    pop %ebx;"
"    pop %esi;"
"    pop %edi;"
"    pop %ebp;"
"    ret"
);

#elif __x86_64__

__asm__ (
".section .text;"
".global z_syscall;"
".internal z_syscall;"
".type z_syscall, @function;"
"z_syscall:"
"    mov %rdi, %rax;"
"    mov %rsi, %rdi;"
"    mov %rdx, %rsi;"
"    mov %rcx, %rdx;"
"    mov %r8, %r10;"
"    mov %r9, %r8;"
"    mov 8(%rsp), %r9;"
"    syscall;"
"    ret"
);

#elif __arm__

__asm__ (
".section .text;"
".global z_syscall;"
".internal z_syscall;"
".type z_syscall, %function;"
"z_syscall:"
"    mov ip, sp;"
"    push {r4, r5, r6, r7};"
"    mov r7, r0;"
"    mov r0, r1;"
"    mov r1, r2;"
"    mov r2, r3;"
"    ldmfd ip, {r3, r4, r5, r6};"
"    swi 0x0;"
"    pop {r4, r5, r6, r7};"
"    bx lr"
);

#elif __aarch64__

__asm__ (
".section .text;"
".global z_syscall;"
".internal z_syscall;"
".type z_syscall, %function;"
"z_syscall:"
"    uxtw x8, w0;"
"    mov x0, x1;"
"    mov x1, x2;"
"    mov x2, x3;"
"    mov x3, x4;"
"    mov x4, x5;"
"    mov x5, x6;"
"    mov x6, x7;"
"    svc 0x0;"
"    ret"
);

#else
#error "unknown arch"
#endif

Z_RESULT_DECLARE(wrapper, long int) z_check_error(long int rc) {
    Z_RESULT(wrapper) r = {rc, 0};

    if (rc < 0 && rc > -4096) {
        r.v = -1;
        r.e = (int)-rc;
    }

    return r;
}

#define SYSCALL(name, ...) z_check_error(z_syscall(SYS_##name, __VA_ARGS__))

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

DEFINE_SYSCALL2(kill, int, pid_t, pid, int, sig)

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

void z_exit_group(int status) {
    SYSCALL(exit_group, status);
}

#if __i386__ || __x86_64__
DEFINE_SYSCALL2(arch_prctl, int, int, code, unsigned long, address);
#endif

#ifdef __aarch64__
Z_RESULT(open) z_open(const char *pathname, int flags, mode_t mode) {
    Z_RESULT(wrapper) r = SYSCALL(openat, AT_FDCWD, pathname, flags, mode);
    return (Z_RESULT(open)) {(int)r.v, r.e};
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
    return (Z_RESULT(stat)) {(int)r.v, r.e};
}
#endif

#if __i386__ || __arm__
Z_RESULT(mmap) z_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    offset = (unsigned long long)offset >> 12;
    Z_RESULT(wrapper) r = SYSCALL(mmap2, addr, length, prot, flags, fd, offset);
    return (Z_RESULT(mmap)) {(void *)r.v, r.e};
}
#else
DEFINE_SYSCALL6(mmap, void *, void *, addr, size_t, length, int, prot, int, flags, int, fd, off_t, offset)
#endif
