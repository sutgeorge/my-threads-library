#include <stdio.h>
#include <ult.h>

void* worker(void* arg) {
    for (int i = 0; i < 1000; i++) {}
}

int main() {
    ult_t th1;
    ult_init(1000); // 1000 microsecond time slice
    ult_create(&th1, worker, NULL);
    printf("Test \"3 - expected proper exit\" passed!\n");
    ult_exit(NULL);

    printf("Test \"3 - expected proper exit\" failed!\n");
    return 0;
}
