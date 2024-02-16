#include <stdio.h>
#include <ult.h>
#include <unistd.h>
#include <stdlib.h>
#define SLEEP_DURATION 10

ult_mutex_t first_lock;
ult_mutex_t second_lock;
int sum = 0;

void* deadlocked_worker_1(void* arg) {
    ult_mutex_lock(&first_lock);
    printf("[%s] sleeping for %d seconds - between locks\n", __FUNCTION__, 1);
    sleep(1);
    printf("[%s] yields the execution - between locks\n", __FUNCTION__);
    ult_yield();
    ult_mutex_lock(&second_lock);

    printf("[%s] acquired lock2 \n", __FUNCTION__);
    sum++;

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
    sum++;

    ult_mutex_unlock(&first_lock);
    printf("[%s] between unlocks\n", __FUNCTION__);
    ult_mutex_unlock(&second_lock);
}

int main() {
    ult_t th1, th2;
    void *ret, *ret2;

    ult_init(1000); // we pick a time slice of 1000 microseconds
    ult_mutex_init(&first_lock);
    ult_mutex_init(&second_lock);

    ult_create(&th1, deadlocked_worker_1, NULL);
    ult_create(&th2, deadlocked_worker_2, NULL);

    ult_join(th1, &ret);
    ult_join(th2, &ret2);

    ult_mutex_destroy(&first_lock);
    ult_mutex_destroy(&second_lock);

    printf("Deadlock test \"1 - tracking edge updates in waits-for graph\" passed!\n");
    return 0;
}
