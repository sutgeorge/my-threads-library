#include <stdio.h>
#include <assert.h>
#include "../src/list.h"

int main() {
    list_t mylist;
    list_init(&mylist);

    list_pushback(&mylist, (void*)6);
    list_pushback(&mylist, (void*)7);
    list_pushback(&mylist, (void*)2);
    assert(list_size(&mylist) == 3);
    assert(list_find(&mylist, (void*)6) == true);
    assert(list_find(&mylist, (void*)2) == true);
    assert(list_find(&mylist, (void*)7) == true);
    assert(list_find(&mylist, (void*)12) == false);

    printf("\033[1;32m");
    printf("Linked list test \"4 - find an element\" passed!\n");
    printf("\033[0m");
    return 0;
}
