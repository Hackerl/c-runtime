#include "z_sync.h"
#include "z_log.h"
#include "z_syscall.h"
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <linux/futex.h>

void z_mutex_lock(z_mutex_t *mutex) {
    while (true) {
        if (__sync_bool_compare_and_swap(mutex, 0, 1))
            break;

        if (z_futex(mutex, FUTEX_WAIT, 1, NULL, NULL, 0) < 0 && z_errno != EAGAIN) {
            LOG("futex wait error: %d", z_errno);
        }
    }
}

void z_mutex_unlock(z_mutex_t *mutex) {
    if (__sync_bool_compare_and_swap(mutex, 1, 0)) {
        if (z_futex(mutex, FUTEX_WAKE, INT_MAX, NULL, NULL, 0) < 0) {
            LOG("futex wake error: %d", z_errno);
        }
    }
}

void z_cond_wait(z_cond_t *cond, z_mutex_t *mutex, const struct timespec *timeout) {
    while (true) {
        if (__sync_bool_compare_and_swap(cond, 1, 0))
            break;

        if (mutex)
            z_mutex_unlock(mutex);

        int r = z_futex(mutex, FUTEX_WAIT, 0, NULL, NULL, 0);

        if (mutex)
            z_mutex_lock(mutex);

        if (r < 0 && z_errno == ETIMEDOUT) {
            break;
        }
    }
}

void z_cond_signal(z_cond_t *cond) {
    if (__sync_bool_compare_and_swap(cond, 0, 1)) {
        if (z_futex(cond, FUTEX_WAKE, 1, NULL, NULL, 0) < 0) {
            LOG("futex wake error: %d", z_errno);
        }
    }
}

void z_cond_broadcast(z_cond_t *cond) {
    if (__sync_bool_compare_and_swap(cond, 0, 1)) {
        if (z_futex(cond, FUTEX_WAKE, INT_MAX, NULL, NULL, 0) < 0) {
            LOG("futex wake error: %d", z_errno);
        }
    }
}