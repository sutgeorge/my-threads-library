#include <stdio.h>
#include <ult.h>

void* worker(void* arg) {
    for (int i = 0; i < 1000; i++) {}
}

int main() {
    ult_t th1;
    ult_init(1000); // 1000 microsecond time slice
    ult_create(&th1, worker, NULL);
    printf("\033[1;32m");
    printf("Test \"3 - expected proper exit\" passed!\n");
    printf("\033[0m");
    ult_exit(NULL);

    printf("\033[0;31m");
    printf("Test \"3 - expected proper exit\" failed!\n");
    printf("\033[0m");
    return 0;
}
