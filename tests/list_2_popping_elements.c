#include <stdio.h>
#include <assert.h>
#include "../src/list.h"

int main() {
    list_t mylist;
    list_init(&mylist);

    list_pushback(&mylist, (void*)5);
    list_pushback(&mylist, (void*)3);
    list_pushback(&mylist, (void*)6);
    list_popfront(&mylist);
    list_popfront(&mylist);
    assert(list_size(&mylist) == 1);
    assert(list_head(&mylist) == (void*)6);

    printf("Linked list test \"2 - popping elements\" passed!\n");
    return 0;
}
