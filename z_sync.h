#ifndef C_RUNTIME_Z_SYNC_H
#define C_RUNTIME_Z_SYNC_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int z_mutex_t;
typedef int z_cond_t;

void z_mutex_lock(z_mutex_t *mutex);
void z_mutex_unlock(z_mutex_t *mutex);

void z_cond_wait(z_cond_t *cond, z_mutex_t *mutex, const struct timespec *timeout);
void z_cond_signal(z_cond_t *cond);
void z_cond_broadcast(z_cond_t *cond);

#ifdef __cplusplus
}
#endif

#endif //C_RUNTIME_Z_SYNC_H
