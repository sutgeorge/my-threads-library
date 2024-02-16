#include <stdio.h>
#include <time.h>
#include <ult.h>
#include <stdlib.h>
#define STEPS 100

ult_mutex_t lock;
int acc = 0;

void* producer_worker(void* arg) {
    for (int i = 0; i < STEPS; i++) {
        ult_mutex_lock(&lock);
        acc++;
        ult_mutex_unlock(&lock);

        int j = 0;
        while (j < rand() % 10000) {j++;} // Keep the producer idle for a random amount of steps
    }
}

void* consumer_worker(void* arg) {
    for (int i = 0; i < STEPS; i++) {
        ult_mutex_lock(&lock);
        acc--;
        ult_mutex_unlock(&lock);

        int j = 0;
        while (j < rand() % 10000) {j++;} // Keep the consumer idle for a random amount of steps
    }
}

int main() {
    ult_t th1, th2;
    srand(time(NULL));

    ult_init(1000);
    ult_mutex_init(&lock);
    ult_create(&th1, producer_worker, NULL);
    ult_create(&th2, consumer_worker, NULL);
    ult_join(th1, NULL);
    ult_join(th2, NULL);

    if (acc != 0) {
        printf("\033[0;31m");
        fprintf(stderr, "Test \"10 - producer-consumer test\" failed. Value: %d\n", acc);
        printf("\033[0m");
        return 0;
    }

    printf("\033[1;32m");
    printf("Test \"10 - producer-consumer test\" passed!\n");
    printf("\033[0m");
    return 0;
}
