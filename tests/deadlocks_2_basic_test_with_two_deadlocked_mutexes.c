#include <stdio.h>
#include <ult.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include "../src/graph.h"
#define SLEEP_DURATION 10

ult_mutex_t first_lock;
ult_mutex_t second_lock;
extern graph_t waits_for_graph;
extern sigset_t vtalrm;
int sum = 0;

bool did_deadlock_occur() {
    sigprocmask(SIG_BLOCK, &vtalrm, NULL);
    printf("[%s] checking if a deadlock exists...\n", __FUNCTION__);
    bool cycle_detected = graph_dfs(&waits_for_graph);
    printf("[%s] deadlock detected? %d\n", __FUNCTION__, cycle_detected);
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    return cycle_detected;
}

void* deadlock_detector_worker(void* arg) {
    bool cycle_detected = false;
    while (true) {
        printf("[%s] running deadlock detection check...\n", __FUNCTION__);
        cycle_detected = did_deadlock_occur();
        printf("[%s] deadlock detection check finished!\n", __FUNCTION__);
        if (cycle_detected) {
            printf("[%s] A cycle was detected, the deadlock detector is terminated.\n", __FUNCTION__);
            break;
        }
        ult_yield();
    }
}

void* deadlocked_worker_1(void* arg) {
    ult_mutex_lock(&first_lock);
    printf("[%s] sleeping for %d seconds - between locks\n", __FUNCTION__, 1);
    sleep(1);
    printf("[%s] yields the execution - between locks\n", __FUNCTION__);
    ult_yield();
    ult_mutex_lock(&second_lock);

    printf("[%s] acquired lock2 \n", __FUNCTION__);
//    sum++;

    ult_mutex_unlock(&second_lock);
    printf("[%s] between unlocks\n", __FUNCTION__);
    ult_mutex_unlock(&first_lock);
}

void* deadlocked_worker_2(void* arg) {
    ult_mutex_lock(&second_lock);
    printf("[%s] sleeping for %d seconds - between locks\n", __FUNCTION__, 1);
    sleep(1);
    printf("[%s] yields the execution - between locks\n", __FUNCTION__);
    ult_yield();
    ult_mutex_lock(&first_lock);

    printf("[%s] acquired lock1 \n", __FUNCTION__);
//    sum++;

    ult_mutex_unlock(&first_lock);
    printf("[%s] between unlocks\n", __FUNCTION__);
    ult_mutex_unlock(&second_lock);
}

int main() {
    ult_t th0, th1, th2;
    void *ret0, *ret1, *ret2;

    ult_init(1000); // we pick a time slice of 1000 microseconds
    ult_mutex_init(&first_lock);
    ult_mutex_init(&second_lock);

    ult_create(&th1, deadlocked_worker_1, NULL);
    ult_create(&th2, deadlocked_worker_2, NULL);
    ult_create(&th0, deadlock_detector_worker, NULL);

    ult_join(th0, &ret0);
    ult_join(th1, &ret1);
    ult_join(th2, &ret2);

    ult_mutex_destroy(&first_lock);
    ult_mutex_destroy(&second_lock);

    printf("Deadlock test \"2 - basic test with two deadlocked mutexes\" passed!\n");
    return 0;
}

