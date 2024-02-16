#include <stdio.h>
#include <ult.h>

#define NUM_THREADS 3

void* worker(void* arg) {
    ult_t th;

    long id = (long) arg;
    printf("Thread %ld created.\n", id);
    if (id < NUM_THREADS) {
        ult_create(&th, worker, (void*) (id + 1));
        ult_join(th, NULL);
    }

    printf("Thread %ld ended.\n", id);
}

int main() {
    ult_t th;

    ult_init(1000);
    ult_create(&th, worker, (void*) 1);
    ult_join(th, NULL);

    printf("\033[1;32m");
    printf("Test \"4 - spawning threads recursively\" passed! (check the output above for any inconsistencies) \n");
    printf("\033[0m");
    return 0;
}
