#include <stdio.h>
#include <ult.h>

void* worker(void* arg) {
    for (int i = 0; i < 1000; i++) {}
}

int main() {
    ult_t th1, th2;
    ult_init(1000);
    ult_create(&th1, worker, (void*)1);
    ult_create(&th2, worker, (void*)1);

    if(ult_equal(th1, th2)) {
        printf("\033[0;31m");
        fprintf(stderr,"Test \"5 - verifying thread ID equality check\" failed. IDs of thread 1 and 2: %ld, %ld\n", th1, th2);
        printf("\033[0m");
        return 0;
    }

    ult_join(th1, NULL);
    ult_join(th2, NULL);

    printf("\033[1;32m");
    printf("Test \"5 - verifying thread ID equality check\" passed!\n");
    printf("\033[0m");
    return 0;
}
