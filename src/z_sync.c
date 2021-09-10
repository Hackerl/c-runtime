#include <z_sync.h>
#include <z_log.h>
#include <z_syscall.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <linux/futex.h>

void z_mutex_lock(z_mutex_t *mutex) {
    while (true) {
        if (__sync_bool_compare_and_swap(mutex, 0, 1))
            break;

        Z_RESULT(futex) r = z_futex(mutex, FUTEX_WAIT, 1, NULL, NULL, 0);

        if (r.v < 0 && r.e != EAGAIN) {
            LOG("futex wait error: %d", r.e);
        }
    }
}

void z_mutex_unlock(z_mutex_t *mutex) {
    if (__sync_bool_compare_and_swap(mutex, 1, 0)) {
        Z_RESULT(futex) r = z_futex(mutex, FUTEX_WAKE, INT_MAX, NULL, NULL, 0);

        if (r.v < 0) {
            LOG("futex wake error: %d", r.e);
        }
    }
}

void z_cond_wait(z_cond_t *cond, z_mutex_t *mutex, const struct timespec *timeout) {
    while (true) {
        if (__sync_bool_compare_and_swap(cond, 1, 0))
            break;

        if (mutex)
            z_mutex_unlock(mutex);

        Z_RESULT(futex) r = z_futex(mutex, FUTEX_WAIT, 0, NULL, NULL, 0);

        if (mutex)
            z_mutex_lock(mutex);

        if (r.v < 0 && r.e == ETIMEDOUT) {
            break;
        }
    }
}

void z_cond_signal(z_cond_t *cond) {
    if (__sync_bool_compare_and_swap(cond, 0, 1)) {
        Z_RESULT(futex) r = z_futex(cond, FUTEX_WAKE, 1, NULL, NULL, 0);

        if (r.v < 0) {
            LOG("futex wake error: %d", r.e);
        }
    }
}

void z_cond_broadcast(z_cond_t *cond) {
    if (__sync_bool_compare_and_swap(cond, 0, 1)) {
        Z_RESULT(futex) r = z_futex(cond, FUTEX_WAKE, INT_MAX, NULL, NULL, 0);

        if (r.v < 0) {
            LOG("futex wake error: %d", r.e);
        }
    }
}

void z_rwlock_read_lock(z_rwlock_t *rwlock) {
    z_mutex_lock(&rwlock->r_mutex);

    if (++rwlock->r_count == 1) {
        z_mutex_lock(&rwlock->w_mutex);
    }

    z_mutex_unlock(&rwlock->r_mutex);
}

void z_rwlock_read_unlock(z_rwlock_t *rwlock) {
    z_mutex_lock(&rwlock->r_mutex);

    if (--rwlock->r_count == 0) {
        z_mutex_unlock(&rwlock->w_mutex);
    }

    z_mutex_unlock(&rwlock->r_mutex);
}

void z_rwlock_write_lock(z_rwlock_t *rwlock) {
    z_mutex_lock(&rwlock->w_mutex);
}

void z_rwlock_write_unlock(z_rwlock_t *rwlock) {
    z_mutex_unlock(&rwlock->w_mutex);
}