#ifndef C_RUNTIME_Z_SYNC_H
#define C_RUNTIME_Z_SYNC_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int z_mutex_t;
typedef int z_cond_t;

typedef struct {
    int r_count;
    z_mutex_t r_mutex;
    z_mutex_t w_mutex;
} z_rwlock_t;

void z_mutex_lock(z_mutex_t *mutex);
void z_mutex_unlock(z_mutex_t *mutex);

void z_cond_wait(z_cond_t *cond, z_mutex_t *mutex, const struct timespec *timeout);
void z_cond_signal(z_cond_t *cond);
void z_cond_broadcast(z_cond_t *cond);

void z_rwlock_read_lock(z_rwlock_t *rwlock);
void z_rwlock_read_unlock(z_rwlock_t *rwlock);
void z_rwlock_write_lock(z_rwlock_t *rwlock);
void z_rwlock_write_unlock(z_rwlock_t *rwlock);

#ifdef __cplusplus
}
#endif

#endif //C_RUNTIME_Z_SYNC_H
