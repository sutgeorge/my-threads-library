#include <stdio.h>
#include <assert.h>
#include "../src/dlist.h"

int main() {
    dlist_t mylist;
    dlist_init(&mylist);

    dlist_pushback(&mylist, (void*)5);
    dlist_pushback(&mylist, (void*)3);
    dlist_pushback(&mylist, (void*)6);
    dlist_popfront(&mylist);
    dlist_popfront(&mylist);
    assert(dlist_size(&mylist) == 1);
    assert(dlist_head(&mylist) == (void*)6);

    printf("Doubly-linked list test \"2 - popping elements\" passed!\n");
    return 0;
}
