#include <stdio.h>
#include <ult.h>
#include <stdlib.h>

void* sum_worker(void* arg) {
    long* numbers = (long*) arg;
    long* result = malloc(sizeof(long));
    *result = numbers[0] + numbers[1];
    return result;
}

int main() {
    ult_t th1;
    long nums[] = {1, 2};
    long expected_sum = 3;
    void* ret;

    ult_init(1000); // we pick a time slice of 1000 microseconds
    ult_create(&th1, sum_worker, (void*) nums);
    ult_join(th1, &ret);

    if (*(long*) ret != expected_sum) {
        fprintf(stderr, "Test \"1 - basic test\" failed. Returned and expected sum: %lx, %lx\n", *(long*)ret, expected_sum);
        return 0;
    }

    printf("Test \"1 - basic test\" passed!\n");
    return 0;
}
