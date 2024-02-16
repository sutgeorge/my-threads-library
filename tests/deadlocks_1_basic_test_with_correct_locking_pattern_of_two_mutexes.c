#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <ult.h>
#include "../src/graph.h"
#define SLEEP_DURATION 10

ult_mutex_t first_lock;
ult_mutex_t second_lock;
extern graph_t waits_for_graph;
extern sigset_t vtalrm;
int sum = 0;

// checks if a deadlock occurs by performing a depth-first search on the waits-for graph that is
// updated every time a thread requests a lock
bool did_deadlock_occur() {
    sigprocmask(SIG_BLOCK, &vtalrm, NULL);
    printf("[%s] checking if a deadlock exists...\n", __FUNCTION__);
    bool cycle_detected = graph_dfs(&waits_for_graph);
    printf("[%s] deadlock detected? %d\n", __FUNCTION__, cycle_detected);
    if (!cycle_detected) {
        printf("\033[1;32m");
        printf("Deadlock test \"1 - basic test with correct locking pattern of two mutexes\" passed!\n");
        printf("\033[0m");
    } else {
        printf("\033[0;31m");
        printf("Deadlock test \"1 - basic test with correct locking pattern of two mutexes\" failed.\n");
        printf("\033[0m");
    }
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    return cycle_detected;
}

void* deadlock_detector_worker(void* arg) {
    bool cycle_detected = false;
    while (true) {
        cycle_detected = did_deadlock_occur();
        if (cycle_detected)
            break;
        ult_yield();
    }
}

void* worker_1(void* arg) {
    ult_mutex_lock(&first_lock);
    printf("[%s] sleeping for %d seconds - between locks\n", __FUNCTION__, 1);
    sleep(1);
    printf("[%s] yields the execution - between locks\n", __FUNCTION__);
    ult_yield();
    ult_mutex_lock(&second_lock);

    printf("[%s] acquired lock2 \n", __FUNCTION__);
    sum++;

    ult_mutex_unlock(&second_lock);
    ult_mutex_unlock(&first_lock);
}

void* worker_2(void* arg) {
    ult_mutex_lock(&first_lock);
    printf("[%s] sleeping for %d seconds - between locks\n", __FUNCTION__, 1);
    sleep(1);
    printf("[%s] yields the execution - between locks\n", __FUNCTION__);
    printf("[%s] did deadlock occur? -> %d\n", __FUNCTION__, did_deadlock_occur());
    ult_mutex_lock(&second_lock);

    printf("[%s] acquired lock2 \n", __FUNCTION__);
    sum++;

    ult_mutex_unlock(&second_lock);
    ult_mutex_unlock(&first_lock);
}

int main() {
    ult_t th0, th1, th2;
    void *ret0, *ret1, *ret2;

    ult_init(1000); // we pick a time slice of 1000 microseconds
    ult_mutex_init(&first_lock);
    ult_mutex_init(&second_lock);

//    ult_create(&th0, deadlock_detector_worker, NULL);
    ult_create(&th1, worker_1, NULL);
    ult_create(&th2, worker_2, NULL);

//    ult_join(th0, &ret0);
    ult_join(th1, &ret1);
    ult_join(th2, &ret2);

    ult_mutex_destroy(&first_lock);
    ult_mutex_destroy(&second_lock);

    return 0;
}
