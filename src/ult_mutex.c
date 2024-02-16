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

    queue_enqueue(mutex, (queue_item) ult_self());
    graph_add_edge(&waits_for_graph, (graph_node) ult_self(), (graph_node) queue_front(mutex));
    printf("DEBUG: thread %ld -> number of edges in the waits-for graph: %d\n", ult_self(), graph_count_edges(&waits_for_graph));
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


    // The following part (all the instructions executed up until the queue_pop operation) ensures that the waits-for
    // graph is properly updated once a thread releases a mutex that is requested by other threads as well.
    // We keep track of the node_index to properly update the graph within a single loop (removing edges between the
    // thread to be popped and a thread that is waiting for the lock, keeping a pointer that references the next thread
    // to be the head of the queue/mutex, adding edges between the rest of the waiting threads and the
    // new head of the queue/mutex).
    queue_node_t* current = mutex->front;
    queue_node_t* new_front_thread = NULL;

    int node_index = 0;
    while (current != NULL) {
        node_index++;
        if (node_index > 1)
            graph_remove_edge(&waits_for_graph, (graph_node) current->item, (graph_node) mutex->front->item);
        if (node_index == 2)
            new_front_thread = current->item;
        if (node_index > 2)
            graph_add_edge(&waits_for_graph, (graph_node) current->item, (graph_node) new_front_thread);

        current = current->next;
    }

    queue_pop(mutex);
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    return 0;
}

// destroys a mutex and returns 0 to indicate a successful mutex destruction, freeing
// up the memory allocated for the queue
int ult_mutex_destroy(ult_mutex_t *mutex) {
    sigprocmask(SIG_BLOCK, &vtalrm, NULL);

    // The following part updates the waits-for graph when a mutex is destroyed (removes edges between the head of
    // the queue and the rest of the threads that were waiting for the lock)
    queue_node_t* current = mutex->front;

    int node_index = 0;
    while (current != NULL) {
        node_index++;
        if (node_index > 1)
            graph_remove_edge(&waits_for_graph, (graph_node) current->item, (graph_node) mutex->front->item);

        current = current->next;
    }

    queue_destroy(mutex);

    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    return 0;
}
