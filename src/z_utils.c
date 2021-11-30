#include <z_utils.h>
#include <z_syscall.h>
#include <z_memory.h>
#include <fcntl.h>

long z_get_file_size(const char *path) {
    int fd = Z_RESULT_V(z_open(path, O_RDONLY, 0));

    if (fd < 0)
        return -1;

    long fs = Z_RESULT_V(z_lseek(fd, 0, SEEK_END));

    if (fs < 0) {
        z_close(fd);
        return -1;
    }

    z_close(fd);

    return fs;
}

int z_read_file(const char *path, char **buffer, size_t *length) {
    long fs = z_get_file_size(path);

    if (fs < 0)
        return -1;

    int fd = Z_RESULT_V(z_open(path, O_RDONLY, 0));

    if (fd < 0)
        return -1;

    char *buf = z_calloc(fs, 1);

    if (!buf) {
        z_close(fd);
        return -1;
    }

    if (Z_RESULT_V(z_read(fd, buf, fs)) != fs) {
        z_free(buf);
        z_close(fd);
        return -1;
    }

    z_close(fd);

    *buffer = buf;
    *length = fs;

    return 0;
}
