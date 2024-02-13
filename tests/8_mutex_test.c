#include <stdio.h>
#include <ult.h>
#define NUM_THREADS 100

ult_mutex_t lock;
int sum = 0;

void* worker(void* arg) {
    ult_mutex_lock(&lock);
    sum++;
    ult_mutex_unlock(&lock);
}

int main() {
    ult_t threads[NUM_THREADS];
    ult_init(1000);
    ult_mutex_init(&lock);
    long i;

    for (i = 0; i < NUM_THREADS; ++i)
        ult_create(&threads[i], worker, (void*) i);
    for (i = 0; i < NUM_THREADS; ++i)
        ult_join(threads[i], NULL);

    if (sum != NUM_THREADS) {
        fprintf(stderr, "Test \"8 - mutex sum test\" failed. Result: %d; Expected value: %d;\n", sum, NUM_THREADS);
        return 0;
    }

    printf("Test \"8 - mutex sum test\" passed!\n");
    return 0;
}
