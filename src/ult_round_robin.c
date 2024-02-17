#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include "ult.h"
#include "list.h"
#include "graph.h"

// thread status constants
#define ULT_RUNNING 0 // running
#define ULT_CANCEL 1 // cancelled
#define ULT_DONE 2 // fully terminated

typedef struct Thread_t {
    ult_t tid; // stores the ID of the thread
    ult_t joining; // stores the join state of the thread
    int state;// stores the state of the thread, containing any of the constants specified above
    void* (*proc)(void*); // stores the address to the routine that contains the thread's instructions
    void* arg; // stores any arguments that might be passed to the routine pointed to by "proc"
    void* retval; // represents the value returned after joining/terminating the thread

    // a data structure used to represent the execution context of a process or thread, containing information about
    // the CPU state at the time of a context switch, allowing the operating system to save and restore the state of
    // a process or thread later on -> https://man7.org/linux/man-pages/man3/getcontext.3.html
    ucontext_t* ucp;
} thread_t;

// A queue that stores the threads to be executed
static queue_t ready_queue;
// The waits-for graph which stores the dependencies between threads, based on the lock request that are performed
graph_t waits_for_graph;

// A queue that stores all of the terminated or cancelled (soon to be terminated) threads -> used in the thread_get
// function to obtain a thread's ID even if it has completed its execution.
static queue_t zombie_queue;
// A variable that stores the context of the current thread that is running
static thread_t* current;
// an interval timer that keeps track of time and expires at some point. In our case, this will be set to the
// ITIMER_VIRTUAL type, for which a SIGVTALRM signal is received by the process when the timer expires
static struct itimerval timer;
// Our signal set: a data structure that stores all the signals, typically represented as a
// collection of bits, with each bit corresponding to a specific signal being enabled or disabled for a particular
// process, suggesting whether it is handled by the process or not).
sigset_t vtalrm;
// keeps track of the maximum existing thread ID and is incremented whenever a thread is created
static ult_t maxtid;

void sigvtalrm_handler(int sig);
void ult_start(void* (*start_routine)(void*), void* args);
thread_t* thread_get(ult_t tid);

// initializes the library and prepares the time slice value in microseconds
void ult_init(long period) {
    // a structure used by a process to change the action taken when receiving a specific signal
    // https://man7.org/linux/man-pages/man2/sigaction.2.html
    struct sigaction act;

    // We initialize the maxtid to 1, the ID of the "main thread"
    maxtid = 1;
    // We initialize the queue of threads that are ready to be processed
    queue_init(&ready_queue);
    graph_init(&waits_for_graph);

    // creates the main thread, adds it to the ready queue to be processed
    // caveat: only the main thread is allocated on the heap and can be finally freed
    thread_t* main_thread = (thread_t*) malloc(sizeof(thread_t));
    main_thread->tid = maxtid++;
    main_thread->ucp = (ucontext_t*) malloc(sizeof(ucontext_t));
    memset(main_thread->ucp, '\0', sizeof(ucontext_t));
    main_thread->arg = NULL;
    main_thread->state = ULT_RUNNING;
    main_thread->joining = 0;

    // checks if the initialization of the main thread's ucp context structures completes successfully
    // (before attempting to call the makecontext function later on)
    if (getcontext(main_thread->ucp) == -1) {
        perror("getcontext");
        exit(EXIT_FAILURE);
    }

    current = main_thread;

    // Here, we initialize the signal set
    sigemptyset(&vtalrm);
    // As the Wikipedia articles explains, "the SIGALRM, SIGVTALRM and SIGPROF signals are sent to a process
    // when the corresponding time limit is reached". What we do here is that we add the flag for the SIGVTALRM
    // signal into the signal set "vtalrm", so that we can disable or enable the handler triggered by the signal
    // when the timer expires, if it is necessary to do so
    sigaddset(&vtalrm, SIGVTALRM);
    // this will remove the signals from the vtalrm set (in case there are any)
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);

    // We set the time slice value passed through the period parameter into the interval timer
    timer.it_interval.tv_usec = period; // value in microseconds for the periodic timer
    timer.it_interval.tv_sec = 0;
    timer.it_value.tv_usec = period; // value in microseconds for the time of the next expiration
    timer.it_value.tv_sec = 0;

    // We set the interval timer with the ITIMER_VIRTUAL type to capture SIGVTALRM signals emitted when the "working
    // time" (CPU time) of the process has elapsed:
    // https://unix.stackexchange.com/questions/588086/what-s-the-difference-between-sigalrm-and-sigvtalrm
    // Essentially, the measured time will represent the CPU time consumed by the process
    if (setitimer(ITIMER_VIRTUAL, &timer, NULL) < 0) {
        perror("setitimer");
        exit(EXIT_FAILURE);
    }

    // initializes all values of the sigaction structure to 0
    memset(&act, '\0', sizeof(act));
    // sets the handler function of the sigaction to the sigvtalrm_handler implemented below in this file
    act.sa_handler = &sigvtalrm_handler;
    // setup for the sigaction which will trigger the handler whenever the process receives the SIGVTALRM signal,
    // with a check for whether the setup is successful or not
    if (sigaction(SIGVTALRM, &act, NULL) < 0) {
        perror ("sigaction");
        exit(EXIT_FAILURE);
    }


}

// similar to pthread_create, the *thread parameter will contain the ID of the created thread,
// while start_routine and arg are the function and its corresponding arguments representing the
// launched thread
int ult_create(ult_t *thread, void *(*start_routine)(void *), void *arg) {
    // blocks the SIGVTALRM signal added into the vtalrm set at initialization time
    sigprocmask(SIG_BLOCK, &vtalrm, NULL);

    // allocates memory for the created thread
    thread_t* t = malloc(sizeof(thread_t));
    *thread = t->tid = maxtid++;
    t->state = ULT_RUNNING;
    t->proc = start_routine;
    t->arg = arg;
    t->ucp = (ucontext_t*) malloc(sizeof(ucontext_t));
    t->joining = 0;
    memset(t->ucp, '\0', sizeof(ucontext_t));

    // initializes the ucp context structure of the thread and checks if this step has completed successfully
    if (getcontext(t->ucp) == -1) {
        perror("getcontext");
        exit(EXIT_FAILURE);
    }

    // We allocate an "alternate stack" for the newly created context, having the size SIGSTKSZ, a constant representing
    // the canonical size for a signal stack (it is judged to be sufficient for normal uses). This alternate stack
    // is used by the signal handler specified in our sigaction structure and function call (there is both a sigaction
    // function and a struct type with the same name).
    //
    // The most common usage of an alternate signal stack is to handle the SIGSEGV signal that is generated
    // if the space available for the standard stack is exhausted: in this case, a signal handler for SIGSEGV
    // cannot be invoked on the standard stack; if we wish to handle it, we must use an alternate signal stack.
    // Source: https://man7.org/linux/man-pages/man2/sigaltstack.2.html#NOTES
    t->ucp->uc_stack.ss_sp = malloc(SIGSTKSZ); // base address of the stack
    t->ucp->uc_stack.ss_size = SIGSTKSZ; // SIGSTKSZ is the canonical size for a signal stack defined in <signal.h>
    t->ucp->uc_stack.ss_flags = 0; // flags
    t->ucp->uc_link = NULL; // points to the context that will be resumed when the current context terminates


    // creates the context, thereby causing the program execution to continue with a call to the ult_start
    // function passed to the "func" function pointer with the start_routine and arg parameters specified
    makecontext(t->ucp, (void (*)(void)) ult_start, 2, start_routine, arg);
    // enqueues/adds the created thread to the queue of threads to be processed

    // adds the thread ID as a node in the waits-for graph
    graph_add_node(&waits_for_graph, t->tid);

    queue_enqueue(&ready_queue, t);

    // unblocks the SIGVTALRM signal contained within the "vtalrm" signal set
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    return 0;
}

// Like pthread_join, this function waits for the thread to terminate. The "status" parameter
// will contain the status of the joined thread after its termination.
int ult_join(ult_t thread, void **status) {
    printf("\033[0;34m");
    printf("[%s] adding edge between threads %lu and %lu\n", __FUNCTION__, ult_self(), thread);
    printf("\033[0m");
    graph_add_edge(&waits_for_graph, (ult_t) ult_self(), (ult_t) thread);
    if (thread == current->tid) { // edge-case for threads that attempt to join themselves
//        graph_remove_edge(&waits_for_graph, (ult_t) ult_self(), thread);
        return -1;
    }

    thread_t* t;
    // if a thread is not yet created, the thread will not be joined
    if ((t = thread_get(thread)) == NULL) {
//        graph_remove_edge(&waits_for_graph, (ult_t) ult_self(), thread);
        return -1;
    }

    // if the thread t is already joining, stop here
    if (t->joining == current->tid) {
//        graph_remove_edge(&waits_for_graph, (ult_t) ult_self(), thread);
        return -1;
    }

    current->joining = t->tid;
    // waits for the thread to terminate
    while (t->state == ULT_RUNNING) {
        sigprocmask(SIG_UNBLOCK, &vtalrm, NULL); // unblocks/enables the SIGVTALRM signal
        sigvtalrm_handler(SIGVTALRM);
        sigprocmask(SIG_BLOCK, &vtalrm, NULL); // blocks/disables the SIGVTALRM signal
    }

//    graph_remove_edge(&waits_for_graph, (ult_t) ult_self(), thread);

    if (status == NULL)
        return 0;

    // sets the given "status" argument with the thread's status value
    if (t->state == ULT_CANCEL)
        *status = (void*) ULT_CANCEL;
    else if (t->state == ULT_DONE)
        *status = t->retval;

    return 0;
}

// Like pthread_exit, this function terminates the calling thread, after which the retval parameter
// will contain the termination status (a value that can be used by other threads in the same process).
void ult_exit(void* retval) {
    sigprocmask(SIG_BLOCK, &vtalrm, NULL); //blocks the SIGVTALRM signal

    // removes the thread ID node from the waits-for graph
    graph_remove_node(&waits_for_graph, ult_self());

    // exits the process if there are no more remaining threads to be processed in the ready queue
    if (queue_isempty(&ready_queue)) {
        sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
        exit((long) retval);
    }

    // if the main thread attempts to exit
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

    // frees up the allocated alternate stack for the signal action's handler
    free(prev->ucp->uc_stack.ss_sp);
    free(prev->ucp);
    prev->ucp = NULL;

    // adds the terminated thread to the zombie queue
    prev->state = ULT_DONE;
    prev->retval = retval;
    prev->joining = 0;
    queue_enqueue(&zombie_queue, prev);

    // unblocks the SIGVTALRM signal and sets the next context
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    setcontext(current->ucp);
}

// Like sched_yield, this function causes the calling thread to relinquish the CPU.
// The thread is moved to the end of the queue for its static
// priority and a new thread gets to run.
int ult_yield(void) {
    sigprocmask(SIG_BLOCK, &vtalrm, NULL); // blocks the SIGVTALRM signal

    if (queue_isempty(&ready_queue)) // returns if the ready queue contains no threads to be yielded
        return 0;

    thread_t* next = (thread_t*) queue_pop(&ready_queue);
    thread_t* prev = current;
    queue_enqueue(&ready_queue, current);
    current = next;

    // unblocks the SIGVTALRM signal
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    // saves the current context into the previous context structure "prev" and activates the context
    // pointed to by "current->ucp"
    swapcontext(prev->ucp, current->ucp);
    return 0;
}

// Compares two thread IDs to determine whether they are identical or not
int ult_equal(ult_t t1, ult_t t2) {
    return t1 == t2;
}

// Sends a cancellation request to a thread (like pthread_cancel)
int ult_cancel(ult_t thread) {
    // if a thread attempts to cancel itself, the thread will exit
    if (ult_equal(current->tid, thread))
        ult_exit(0);

    sigprocmask(SIG_BLOCK, &vtalrm, NULL); // blocks the SIGVTALRM signal
    // gets the thread corresponding to the thread ID specified in the given parameter
    thread_t* t = thread_get(thread);

    // if the thread to be cancelled is non-existent, we return -1 to indicate failure
    if (t == NULL) {
        sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
        return -1;
    }

    // if the thread to be cancelled is in the DONE state, we return -1 to indicate failure
    if (t->state == ULT_DONE) {
        sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
        return -1;
    }

    // if the thread to be cancelled is in the CANCEL state, we return -1 to indicate failure
    if (t->state == ULT_CANCEL) {
        sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
        return -1;
    } else {
        // the state of the thread is set to ULT_CANCEL
        t->state = ULT_CANCEL;
    }

    // This part frees up the memory allocated for the alternate stack used by the signal action handler
    free(t->ucp->uc_stack.ss_sp);
    free(t->ucp);

    t->ucp = NULL;
    t->joining = 0;

    // adds the thread to the zombie queue
    queue_enqueue(&zombie_queue, t);

    // unblocks the SIGVTALRM signal
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    return 0;
}

// Like pthread_self, it returns the ID of the calling thread
ult_t ult_self(void) { return current->tid; }

// This function is a wrapper around the function call that represents the thread starting point
void ult_start(void* (*start_routine)(void*), void* args) {
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL); // unblocks the SIGVTALRM signal
    current->retval = (*start_routine)(args); // runs the given routine
    ult_exit(current->retval); // exits the executed thread
}

// a signal handler called by the round-robin scheduler and the mutexes to handle the SIGVTALRM signal
// emitted when the interval timer measuring the elapsed CPU time expires
void sigvtalrm_handler(int sig) {
    sigprocmask(SIG_BLOCK, &vtalrm, NULL); // blocks the SIGVTALRM signal

    // if the ready queue contains no threads, the execution is not affected
    if (queue_isempty(&ready_queue))
        return;

    // This part represents the preemptive scheduling that is performed.
    // The next thread that has to be executed is obtained by searching for a thread within the
    // linked-list-based queue and stopping only after the "next" pointer identifies a thread with a
    // state not equal to ULT_CANCEL
    thread_t* next = (thread_t*) queue_pop(&ready_queue);
    while (next->state == ULT_CANCEL) {
        queue_enqueue(&zombie_queue, next);
        next = (thread_t*) queue_pop(&ready_queue);
    }

    thread_t* prev = current;
    queue_enqueue(&ready_queue, current);
    next->state = ULT_RUNNING;
    current = next;

    // unblocks the SIGVTALRM signal
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    // saves the current context in the "prev" structure and continues with the one contained in the "current" structure
    swapcontext(prev->ucp, current->ucp);
}

// Given a thread ID, this function searches the thread in ready_queue and zombie queue, it is useful
// when attempting to find out whether the thread to be joined was created before.
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
