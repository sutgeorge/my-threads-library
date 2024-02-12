#ifndef __ULT_H
#define __ULT_H

#include <ucontext.h>
#include "queue.h"
#include <signal.h>

typedef unsigned long int ult_t;
typedef queue_t ult_mutex_t;

void ult_init(long period);
int ult_create(ult_t *thread, void *(*start_routine)(void *), void *arg);
int ult_join(ult_t thread, void **status);
void ult_exit(void *retval);
int ult_yield(void);
int ult_equal(ult_t t1, ult_t t2);
int ult_cancel(ult_t thread);
ult_t ult_self(void);

int ult_mutex_init(ult_mutex_t *mutex);
int ult_mutex_lock(ult_mutex_t *mutex);
int ult_mutex_unlock(ult_mutex_t *mutex);
int ult_mutex_destroy(ult_mutex_t *mutex);
void sigvtalrm_handler(int sig);

#endif // __ULT_H
