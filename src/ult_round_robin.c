#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include "ult.h"
#include "queue.h"

#define ULT_RUNNING 0
#define ULT_CANCEL 1
#define ULT_DONE 2

typedef struct Thread_t {
    ult_t tid;
    ult_t joining;
    int state;
    void* (*proc)(void*);
    void* arg;
    void* retval;
    ucontext_t* ucp;
} thread_t;

static queue_t ready_queue;
static queue_t zombie_queue;
static thread_t* current;
static struct itimerval timer;
sigset_t vtalrm;
static ult_t maxtid;

void sigvtalrm_handler(int sig);
void ult_start(void* (*start_routine)(void*), void* args);
thread_t* thread_get(ult_t tid);

void ult_init(long period) {
    struct sigaction act;

    maxtid = 1;
    queue_init(&ready_queue);

    thread_t* main_thread = (thread_t*) malloc(sizeof(thread_t));
    main_thread->tid = maxtid++;
    main_thread->ucp = (ucontext_t*) malloc(sizeof(ucontext_t));
    memset(main_thread->ucp, '\0', sizeof(ucontext_t));
    main_thread->arg = NULL;
    main_thread->state = ULT_RUNNING;
    main_thread->joining = 0;

    if (getcontext(main_thread->ucp) == -1) {
        perror("getcontext");
        exit(EXIT_FAILURE);
    }

    current = main_thread;

    sigemptyset(&vtalrm);
    sigaddset(&vtalrm, SIGVTALRM);
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);

    timer.it_interval.tv_usec = period;
    timer.it_interval.tv_sec = 0;
    timer.it_value.tv_usec = period;
    timer.it_value.tv_sec = 0;

    if (setitimer(ITIMER_VIRTUAL, &timer, NULL) < 0) {
        perror("setitimer");
        exit(EXIT_FAILURE);
    }

    memset(&act, '\0', sizeof(act));
    act.sa_handler = &sigvtalrm_handler;
    if (sigaction(SIGVTALRM, &act, NULL) < 0) {
        perror ("sigaction");
        exit(EXIT_FAILURE);
    }
}


int ult_create(ult_t *thread, void *(*start_routine)(void *), void *arg) {
    sigprocmask(SIG_BLOCK, &vtalrm, NULL);

    thread_t* t = malloc(sizeof(thread_t));
    *thread = t->tid = maxtid++;
    t->state = ULT_RUNNING;
    t->proc = start_routine;
    t->arg = arg;
    t->ucp = (ucontext_t*) malloc(sizeof(ucontext_t));
    t->joining = 0;
    memset(t->ucp, '\0', sizeof(ucontext_t));

    if (getcontext(t->ucp) == -1) {
        perror("getcontext");
        exit(EXIT_FAILURE);
    }

    t->ucp->uc_stack.ss_sp = malloc(SIGSTKSZ);
    t->ucp->uc_stack.ss_size = SIGSTKSZ;
    t->ucp->uc_stack.ss_flags = 0;
    t->ucp->uc_link = NULL;

    makecontext(t->ucp, (void (*)(void)) ult_start, 2, start_routine, arg);
    queue_enqueue(&ready_queue, t);

    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    return 0;
}

int ult_join(ult_t thread, void **status) {
    if (thread == current->tid)
        return -1;

    thread_t* t;
    if ((t = thread_get(thread)) == NULL)
        return -1;

    if (t->joining == current->tid)
        return -1;

    current->joining = t->tid;
    while (t->state == ULT_RUNNING) {
        sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
        sigvtalrm_handler(SIGVTALRM);
        sigprocmask(SIG_BLOCK, &vtalrm, NULL);
    }

    if (status == NULL)
        return 0;

    if (t->state == ULT_CANCEL)
        *status = (void*) ULT_CANCEL;
    else if (t->state == ULT_DONE)
        *status = t->retval;

    return 0;
}

void ult_exit(void* retval) {
    sigprocmask(SIG_BLOCK, &vtalrm, NULL);

    if (queue_isempty(&ready_queue)) {
        sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
        exit((long) retval);
    }

    if (current->tid == 1) {
        while (!queue_isempty(&ready_queue)) {
            sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
            sigvtalrm_handler(SIGVTALRM);
            sigprocmask(SIG_BLOCK, &vtalrm, NULL);
        }

        sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
        exit((long) retval);
    }

    thread_t* prev = current;
    current = (thread_t*) queue_pop(&ready_queue);
    current->state = ULT_RUNNING;

    free(prev->ucp->uc_stack.ss_sp);
    free(prev->ucp);
    prev->ucp = NULL;

    prev->state = ULT_DONE;
    prev->retval = retval;
    prev->joining = 0;
    queue_enqueue(&zombie_queue, prev);

    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    setcontext(current->ucp);
}

int ult_yield(void) {
    sigprocmask(SIG_BLOCK, &vtalrm, NULL);

    if (queue_isempty(&ready_queue))
        return 0;

    thread_t* next = (thread_t*) queue_pop(&ready_queue);
    thread_t* prev = current;
    queue_enqueue(&ready_queue, current);
    current = next;

    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    swapcontext(prev->ucp, current->ucp);
    return 0;
}

int  ult_equal(ult_t t1, ult_t t2) {
    return t1 == t2;
}

int ult_cancel(ult_t thread) {
    if (ult_equal(current->tid, thread))
        ult_exit(0);

    sigprocmask(SIG_BLOCK, &vtalrm, NULL);
    thread_t* t = thread_get(thread);
    if (t == NULL) {
        sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
        return -1;
    }

    if (t->state == ULT_DONE) {
        sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
        return -1;
    }

    if (t->state == ULT_CANCEL) {
        sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
        return -1;
    } else {
        t->state = ULT_CANCEL;
    }

    free(t->ucp->uc_stack.ss_sp);
    free(t->ucp);
    t->ucp = NULL;
    t->joining = 0;
    queue_enqueue(&zombie_queue, t);
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    return 0;
}

ult_t ult_self(void) { return current->tid; }

void ult_start(void* (*start_routine)(void*), void* args) {
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    current->retval = (*start_routine)(args);
    ult_exit(current->retval);
}

void sigvtalrm_handler(int sig) {
    sigprocmask(SIG_BLOCK, &vtalrm, NULL);

    if (queue_isempty(&ready_queue))
        return;

    thread_t* next = (thread_t*) queue_pop(&ready_queue);
    while (next->state == ULT_CANCEL) {
        queue_enqueue(&zombie_queue, next);
        next = (thread_t*) queue_pop(&ready_queue);
    }

    thread_t* prev = current;
    queue_enqueue(&ready_queue, current);
    next->state = ULT_RUNNING;
    current = next;

    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    swapcontext(prev->ucp, current->ucp);
}

thread_t* thread_get(ult_t tid) {
    queue_node_t* current = ready_queue.front;
    while (current != NULL) {
        thread_t* t= (thread_t*) current->item;
        if (t->tid == tid)
            return t;
        current = current->next;
    }

    current = zombie_queue.front;
    while (current != NULL) {
        thread_t* t= (thread_t*) current->item;
        if (t->tid == tid)
            return t;
        current = current->next;
    }
    return NULL;
}
