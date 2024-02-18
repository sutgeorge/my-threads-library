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
    bool cycle_detected = graph_dfs(&waits_for_graph);
    printf("\033[31;m");
    printf("[%s] deadlock detected? %d\n", __FUNCTION__, cycle_detected);
    printf("\033[0m");
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    return cycle_detected;
}

void* deadlocked_worker_1(void* arg) {
    ult_t* thread_2 = (ult_t*) arg;

    printf("[%s] - thread %lu joins with thread %lu\n", __FUNCTION__, ult_self(), *thread_2);

    ult_join(*thread_2, NULL);

    ult_yield();
}

void* deadlocked_worker_2(void* arg) {
    ult_t* thread_1 = (ult_t*) arg;

    printf("[%s] - thread %lu joins with thread %lu\n", __FUNCTION__, ult_self(), *thread_1);

    ult_join(*thread_1, NULL);

    bool deadlocked = did_deadlock_occur();
    printf("[%s] - thread %lu - Did a deadlock occur? --> %d \n", __FUNCTION__, ult_self(), deadlocked);
    printf("[%s] - thread %lu yields the execution \n", __FUNCTION__, ult_self());
    ult_yield();
}

int main() {
    ult_t th0, th1, th2, th3;
    void *ret0, *ret1, *ret2, *ret3;

    ult_init(1000); // we pick a time slice of 1000 microseconds
    ult_mutex_init(&first_lock);
    ult_mutex_init(&second_lock);

    ult_create(&th1, deadlocked_worker_1, &th2);
    ult_create(&th2, deadlocked_worker_2, &th1);

    ult_join(th1, &ret1);
    ult_join(th2, &ret2);

    ult_mutex_destroy(&first_lock);
    ult_mutex_destroy(&second_lock);

    return 0;
}

