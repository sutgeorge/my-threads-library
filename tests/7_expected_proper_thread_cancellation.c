#include <stdio.h>
#include <ult.h>
#include <stdbool.h>
#include <stdlib.h>

void* infinite_worker_2(void* arg) {
    while (true) {}
}

void* infinite_worker_1(void* arg) {
    ult_t th;
    ult_create(&th, infinite_worker_2, NULL);

    if(ult_cancel(th) == -1) {
        printf("Test \"7 - expected proper thread cancellation check\" failed!\n");
        exit(EXIT_FAILURE);
    }

    ult_join(th, NULL);
    while (true) {}
}

int main() {
    ult_t th;
    ult_init(1000);
    ult_create(&th, infinite_worker_1, NULL);

    if(ult_cancel(th) == -1) {
        printf("\033[0;31m");
        printf("Test \"7 - expected proper thread cancellation check\" failed!\n");
        printf("\033[0m");
        exit(EXIT_FAILURE);
    }

    ult_join(th, NULL);

    printf("\033[1;32m");
    printf("Test \"7 - expected proper thread cancellation check\" passed!\n");
    printf("\033[0m");
    return 0;
}
