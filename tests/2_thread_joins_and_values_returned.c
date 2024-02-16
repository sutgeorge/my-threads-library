#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ult.h>
#define NUM_THREADS 25

long values[NUM_THREADS];

void* worker(void* arg) {
    long i = (long) arg;
    ult_exit((void*) values[i]);
    return NULL;
}

int main() {
    srand(time(NULL));
    ult_t threads[NUM_THREADS];
    void* rets[NUM_THREADS];
    long i;

    for (i = 0; i < NUM_THREADS; ++i)
        values[i] = rand();
    ult_init(1000); // initialize time slice of 1000 microseconds
    for (i = 0; i < NUM_THREADS; ++i)
        ult_create(&threads[i], worker, (void*) i);
    for (i = 0; i < NUM_THREADS; ++i)
        ult_join(threads[i], &rets[i]);

    for (i = 0; i < NUM_THREADS; ++i)
        if ((long) rets[i] != values[i]) {
            printf("\033[0;31m");
            fprintf(stderr,"Test \"2 - thread joining and values returned\" failed. The %ld-th returned value %ld is incorrect. Expected value: %ld\n", i, (long) rets[i], values[i]);
            printf("\033[0m");
            return 0;
        }

    printf("\033[1;32m");
    printf("Test \"2 - thread joining and values returned\" passed!\n");
    printf("\033[0m");
    return 0;
}
