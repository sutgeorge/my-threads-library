#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ult.h"

extern sigset_t vtalrm;

// initializes a mutex and returns 0 to indicate a successful initialization
int ult_mutex_init(ult_mutex_t* mutex) {
    sigprocmask(SIG_BLOCK, &vtalrm, NULL);
    queue_init(mutex);
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    return 0;
}

// acquires a mutex and returns 0 to indicate a successful mutex lock
int ult_mutex_lock(ult_mutex_t* mutex){
    sigprocmask(SIG_BLOCK, &vtalrm, NULL);

    if (queue_isempty(mutex)) {
        queue_enqueue(mutex, (queue_item) ult_self());
        sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
        return 0;
    }

    if (ult_self() == (ult_t) queue_front(mutex)) {
        sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
        return 0;
    }

    queue_enqueue(mutex, (queue_item) ult_self());
    while (ult_self() != (ult_t) queue_front(mutex)) {
        sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
        sigvtalrm_handler(SIGVTALRM);
        sigprocmask(SIG_BLOCK, &vtalrm, NULL);
    }

    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    return 0;
}

// releases a mutex and returns 0 to indicate a successful mutex unlock
int ult_mutex_unlock(ult_mutex_t *mutex){
    sigprocmask(SIG_BLOCK, &vtalrm, NULL);
    if (queue_isempty(mutex)) {
        sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
        return -1;
    }

    if ((ult_t) queue_front(mutex) != ult_self()) {
        sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
        return -1;
    }

    queue_pop(mutex);
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    return 0;
}

// destroys a mutex and returns 0 to indicate a successful mutex destruction, freeing
// up the memory allocated for the queue
int ult_mutex_destroy(ult_mutex_t *mutex){
    sigprocmask(SIG_BLOCK, &vtalrm, NULL);
    queue_destroy(mutex);
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    return 0;
}
