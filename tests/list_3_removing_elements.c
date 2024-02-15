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
    list_remove(&mylist, (void*)3);
    assert(list_size(&mylist) == 2);
    list_remove(&mylist, (void*)5);
    assert(list_size(&mylist) == 1);
    assert(list_head(&mylist) == (void*)6);

    printf("Linked list test \"3 - removing elements\" passed!\n");
    return 0;
}
