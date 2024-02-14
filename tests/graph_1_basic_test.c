#include <stdio.h>
#include <assert.h>
#include "../src/graph.h"

int main() {
    graph_t mygraph;
    graph_init(&mygraph);

    graph_add_node(&mygraph, (void*)1);
    graph_add_node(&mygraph, (void*)2);
    graph_add_node(&mygraph, (void*)3);
    graph_add_edge(&mygraph, (void*)2, (void*)1);
    graph_add_edge(&mygraph, (void*)1, (void*)2);
    graph_destroy(&mygraph);

    printf("Graph test \"1 - basic test\" passed!\n");
    return 0;
}
