#include <z_log.h>
#include <z_syscall.h>
#include <unistd.h>

void _putchar(char character) {
    z_write(STDOUT_FILENO, &character, 1);
}