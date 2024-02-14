#include <stdio.h>
#include <assert.h>
#include "../src/dlist.h"

int main() {
    dlist_t mylist;
    dlist_init(&mylist);

    dlist_pushback(&mylist, (void*)5);
    dlist_pushback(&mylist, (void*)3);
    dlist_pushback(&mylist, (void*)6);
    assert(dlist_size(&mylist) == 3);

    printf("Doubly-linked list test \"1 - basic test\" passed!\n");
    return 0;
}