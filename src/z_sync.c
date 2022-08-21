#include <z_sync.h>
#include <z_log.h>
#include <z_syscall.h>
#include <z_memory.h>
#include <z_std.h>
#include <stdint.h>
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

        Z_RESULT(futex) r = z_futex(cond, FUTEX_WAIT, 0, timeout, NULL, 0);

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

void z_circular_buffer_init(z_circular_buffer_t *buffer, size_t size, size_t length) {
    buffer->head = 0;
    buffer->tail = 0;
    buffer->size = size;
    buffer->length = length;

    buffer->buffer = z_calloc(length, size);
    buffer->state = z_calloc(length, sizeof(z_circular_buffer_state_t));
}

void z_circular_buffer_destroy(z_circular_buffer_t *buffer) {
    z_free(buffer->buffer);
    z_free(buffer->state);
}

size_t z_circular_buffer_size(z_circular_buffer_t *buffer) {
    return (__atomic_load_n(&buffer->tail, __ATOMIC_SEQ_CST) % buffer->length + buffer->length - __atomic_load_n(&buffer->head, __ATOMIC_SEQ_CST) % buffer->length) % buffer->length;
}

bool z_circular_buffer_full(z_circular_buffer_t *buffer) {
    return (__atomic_load_n(&buffer->tail, __ATOMIC_SEQ_CST) + 1) % buffer->length == __atomic_load_n(&buffer->head, __ATOMIC_SEQ_CST) % buffer->length;
}

bool z_circular_buffer_empty(z_circular_buffer_t *buffer) {
    return __atomic_load_n(&buffer->head, __ATOMIC_SEQ_CST) == __atomic_load_n(&buffer->tail, __ATOMIC_SEQ_CST);
}

bool z_circular_buffer_enqueue(z_circular_buffer_t *buffer, const void *element) {
    size_t index = __atomic_load_n(&buffer->tail, __ATOMIC_SEQ_CST);

    do {
        if (z_circular_buffer_full(buffer))
            return false;
    } while (!__atomic_compare_exchange_n(&buffer->tail, &index, (index + 1) % (SIZE_MAX - (SIZE_MAX % buffer->length)), true, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST));

    index %= buffer->length;

    while (!__sync_bool_compare_and_swap(&buffer->state[index], IDLE, PUTTING)) {

    }

    z_memcpy((char *)buffer->buffer + index * buffer->size, element, buffer->size);

    __atomic_store_n(&buffer->state[index], VALID, __ATOMIC_SEQ_CST);

    return true;
}

bool z_circular_buffer_dequeue(z_circular_buffer_t *buffer, void *element) {
    size_t index = __atomic_load_n(&buffer->head, __ATOMIC_SEQ_CST);

    do {
        if (z_circular_buffer_empty(buffer))
            return false;
    } while (!__atomic_compare_exchange_n(&buffer->head, &index, (index + 1) % (SIZE_MAX - (SIZE_MAX % buffer->length)), true, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST));

    index %= buffer->length;

    while (!__sync_bool_compare_and_swap(&buffer->state[index], VALID, TAKING)) {

    }

    z_memcpy(element, (char *)buffer->buffer + index * buffer->size, buffer->size);

    __atomic_store_n(&buffer->state[index], IDLE, __ATOMIC_SEQ_CST);

    return true;
}