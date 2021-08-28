#ifndef DO_SYSCALL_H
#define DO_SYSCALL_H

#ifdef __cplusplus
extern "C" {
#endif

long int do_syscall(long int sys_no, ...);

#ifdef __cplusplus
}
#endif

#ifdef __i386__

__asm__ (
".section .text\n"
".type do_syscall, @function\n"
"do_syscall:\n"
"    push %ebp\n"
"    push %edi\n"
"    push %esi\n"
"    push %ebx\n"
"    mov 44(%esp), %ebp\n"
"    mov 40(%esp), %edi\n"
"    mov 36(%esp), %esi\n"
"    mov 32(%esp), %edx\n"
"    mov 28(%esp), %ecx\n"
"    mov 24(%esp), %ebx\n"
"    mov 20(%esp), %eax\n"
"    int $0x80\n"
"    pop %ebx\n"
"    pop %esi\n"
"    pop %edi\n"
"    pop %ebp\n"
"    ret"
);

#elif __x86_64__

__asm__ (
".section .text\n"
".type do_syscall, @function\n"
"do_syscall:\n"
"    mov %rdi, %rax\n"
"    mov %rsi, %rdi\n"
"    mov %rdx, %rsi\n"
"    mov %rcx, %rdx\n"
"    mov %r8, %r10\n"
"    mov %r9, %r8\n"
"    mov 8(%rsp), %r9\n"
"    syscall\n"
"    ret"
);

#elif __arm__

__asm__ (
".section .text\n"
".type do_syscall, %function\n"
"do_syscall:\n"
"    mov ip, sp\n"
"    push {r4, r5, r6, r7}\n"
"    mov r7, r0\n"
"    mov r0, r1\n"
"    mov r1, r2\n"
"    mov r2, r3\n"
"    ldmfd ip, {r3, r4, r5, r6}\n"
"    swi 0x0\n"
"    pop {r4, r5, r6, r7}\n"
"    bx lr"
);

#elif __aarch64__

__asm__ (
".section .text\n"
".type do_syscall, %function\n"
"do_syscall:\n"
"    uxtw x8, w0\n"
"    mov x0, x1\n"
"    mov x1, x2\n"
"    mov x2, x3\n"
"    mov x3, x4\n"
"    mov x4, x5\n"
"    mov x5, x6\n"
"    mov x6, x7\n"
"    svc 0x0\n"
"    ret"
);

#else
#error "unknown arch"
#endif

#endif