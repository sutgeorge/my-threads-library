#include <stdio.h>
#include <ult.h>
ult_t th1, th2;

void* expected_join_failed_worker1(void* arg) {
    if (ult_join(th2, NULL) == -1)
        printf("Joining error.\n");
}

void* expected_join_failed_worker2(void* arg) {
    int i;
    for(i = 0; i < 99999999; ++i);
    if (ult_join(th1, NULL) == -1)
        printf("Joining error.\n");
}

int main() {
    ult_init(1000);
    ult_create(&th2, expected_join_failed_worker2, (void*)2);
    ult_create(&th1, expected_join_failed_worker1, (void*)1);
    ult_join(th1, NULL);

    printf("\033[1;32m");
    printf("Test \"11 - expected join error test\" passed! (check the above output: a joining error is expected)\n");
    printf("\033[0m");
    return 0;
}
