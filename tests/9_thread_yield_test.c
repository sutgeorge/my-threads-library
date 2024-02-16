#include <stdio.h>
#include <ult.h>
#define NUM_THREADS 3

void* worker(void* arg) {
    printf("Thread %ld started.\n", (long)arg);
    ult_yield(); // yields the execution to another thread
    printf("Thread %ld resumed.\n", (long)arg);
}

int main() {
    ult_t th[NUM_THREADS];
    ult_init(1000);

    for (int i = 0; i < NUM_THREADS; i++)
        ult_create(&th[i], worker, (void*)(long)(i+1));
    for (int i = 0; i < NUM_THREADS; i++)
        ult_join(th[i], NULL);

    printf("\033[1;32m");
    printf("Test \"9 - thread yield operation test\" passed! (check the output above, it should be 1-2-3-1-2-3)\n");
    printf("\033[0m");
    return 0;
}
