#include <stdio.h>
#include <assert.h>
#include "../src/graph.h"

int main() {
    graph_t mygraph;
    graph_init(&mygraph);

    graph_add_node(&mygraph, (void*)1);
    graph_add_node(&mygraph, (void*)2);
    graph_add_node(&mygraph, (void*)4);
    graph_add_node(&mygraph, (void*)6);
    graph_add_node(&mygraph, (void*)8);
    graph_add_node(&mygraph, (void*)10);

    assert(graph_count_nodes(&mygraph) == 6);

    graph_destroy(&mygraph);

    printf("\033[1;32m");
    printf("Graph test \"2 - count nodes explicitly\" passed!\n");
    printf("\033[0m");
    return 0;
}
