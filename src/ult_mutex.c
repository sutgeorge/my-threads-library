#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ult.h"
#include "list.h"
#include "graph.h"

extern sigset_t vtalrm; // check the variable defined in ult_round_robin.c
extern graph_t waits_for_graph; // check the variable defined in ult_round_robin.c

// initializes a mutex and returns 0 to indicate a successful initialization
int ult_mutex_init(ult_mutex_t* mutex) {
    sigprocmask(SIG_BLOCK, &vtalrm, NULL);
    queue_init(mutex);
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    return 0;
}

// acquires a mutex and returns 0 to indicate a successful mutex lock
int ult_mutex_lock(ult_mutex_t* mutex) {
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

    graph_add_edge(&waits_for_graph, (graph_node) ult_self(), (graph_node) queue_back(mutex));
    queue_enqueue(mutex, (queue_item) ult_self());
    // we could actually execute the deadlock detection function in this loop and terminate the threads in the
    // cycle if we wish to do so, but that is an idea for a potential improvement (deadlock resolution)
    int debug_print_count = 0;
    // In case of a simple mutex-based deadlock, this loop will essentially run indefinitely -> this is one of the
    // possible causes of compiler-generated errors like "***stack smashing detected***" or SIGSEGV/segmentation faults
    // -> the loop runs continuously, the SIGVTALRM handler function is called and therefore the memory that is used
    // (such as the zombie queue which stores terminated threads and the ready queue that will store the same thread IDs
    // whose contexts are swapped) is overflown with the same data (in our case, the thread ID pushed onto the queue).
    // As a future improvement, it would probably make sense to prevent the overallocation of memory in such cases.
    while (ult_self() != (ult_t) queue_front(mutex)) {
        sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
        if (debug_print_count < 1)
            printf("[%s] debug 8 - thread %lu - mutex owner: %p \n", __FUNCTION__, ult_self(), mutex->front->item);
        sigvtalrm_handler(SIGVTALRM);

        if (debug_print_count < 1)
            printf("[%s] debug 9 - thread %lu - mutex owner: %p \n", __FUNCTION__, ult_self(), mutex->front->item);

        sigprocmask(SIG_BLOCK, &vtalrm, NULL);
        if (debug_print_count < 1)
            printf("[%s] debug 10 - thread %lu - mutex owner: %p \n", __FUNCTION__, ult_self(), mutex->front->item);

        debug_print_count++;
//        printf("[%s] debug 11 - thread %lu - mutex owner: %p - debug_print_count: %d \n", __FUNCTION__, ult_self(), mutex->front->item, debug_print_count);
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

    if (mutex->N >= 2)
        graph_remove_edge(&waits_for_graph, (graph_node) mutex->front->next->item, (graph_node) mutex->front->item);

    queue_pop(mutex);
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    return 0;
}

// destroys a mutex and returns 0 to indicate a successful mutex destruction, freeing
// up the memory allocated for the queue
int ult_mutex_destroy(ult_mutex_t *mutex) {
    sigprocmask(SIG_BLOCK, &vtalrm, NULL);

    // The following part updates the waits-for graph when a mutex is destroyed (removes edges between consecutive
    // threads in the queue)
    queue_node_t* current = mutex->front;

    while (current != NULL && current->next != NULL) {
        graph_remove_edge(&waits_for_graph, (graph_node) current->next->item, (graph_node) current->item);
        current = current->next;
    }

    queue_destroy(mutex);

    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    return 0;
}
