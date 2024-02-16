#include <stdio.h>
#include <assert.h>
#include "../src/list.h"

int main() {
    list_t mylist;
    list_init(&mylist);

    list_pushback(&mylist, (void*)5);
    list_pushback(&mylist, (void*)3);
    list_pushback(&mylist, (void*)6);
    assert(list_size(&mylist) == 3);

    printf("\033[1;32m");
    printf("Linked list test \"1 - basic test\" passed!\n");
    printf("\033[0m");
    return 0;
}