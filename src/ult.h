#ifndef __ULT_H
#define __ULT_H

#include <ucontext.h>
#include "queue.h"
#include <signal.h>

// a thread is by default represented as an "unsigned long int", just like most platforms define the pthread_t type in
// the pthread library
typedef unsigned long int ult_t;

// type representing a mutex as a queue
typedef queue_t ult_mutex_t;

// initializes the library and prepares the time slice value in microseconds
void ult_init(long period);

// similar to pthread_create, the *thread parameter will contain the ID of the created thread,
// while start_routine and arg are the function and its corresponding arguments representing the
// launched thread
int ult_create(ult_t *thread, void *(*start_routine)(void *), void *arg);

// Like pthread_join, this function waits for the thread to terminate. The "status" parameter
// will contain the status of the joined thread after its termination.
int ult_join(ult_t thread, void **status);

// Like pthread_exit, this function terminates the calling thread, after which the retval parameter
// will contain the termination status (a value that can be used by other threads in the same process).
void ult_exit(void *retval);

// Like sched_yield, this function causes the calling thread to relinquish the CPU.
// The thread is moved to the end of the queue for its static
// priority and a new thread gets to run.
int ult_yield(void);

// Compares two thread IDs to determine whether they are identical or not
int ult_equal(ult_t t1, ult_t t2);

// Sends a cancellation request to a thread (like pthread_cancel)
int ult_cancel(ult_t thread);

// Like pthread_self, it returns the ID of the calling thread
ult_t ult_self(void);


// initializes a mutex
int ult_mutex_init(ult_mutex_t *mutex);

// acquires a mutex
int ult_mutex_lock(ult_mutex_t *mutex);

// releases a mutex
int ult_mutex_unlock(ult_mutex_t *mutex);

// destroys a mutex (frees up allocated memory)
int ult_mutex_destroy(ult_mutex_t *mutex);

// a signal handler called by the round-robin scheduler and the mutexes
void sigvtalrm_handler(int sig);

#endif // __ULT_H
