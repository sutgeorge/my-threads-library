#include <stdio.h>
#include <ult.h>

ult_t g_th1, g_th2;

int result[4];

void* worker(void* arg) {
    ult_t tid = ult_self();
    result[tid] = ult_equal(g_th2, tid);
}

int main() {
    int rc;

    ult_init(1000);

    rc = ult_create(&g_th1, worker, NULL);
    rc = ult_create(&g_th2, worker, NULL);

    ult_join(g_th1, NULL);
    ult_join(g_th2, NULL);

    if (result[g_th1] != 0 || result[g_th2] != 1) {
        printf("\033[0;31m");
        fprintf(stderr,"Test \"6 - verifying thread ID equality check\" failed.\n");
        printf("\033[0m");
        return 0;
    }

    printf("\033[1;32m");
    printf("Test \"6 - verifying thread ID equality check\" passed!\n");
    printf("\033[0m");
    return 0;
}
