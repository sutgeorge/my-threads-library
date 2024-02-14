#include <stdio.h>
#include <assert.h>
#include "../src/graph.h"

int main() {
    graph_t mygraph;
    graph_init(&mygraph);

    graph_add_node(&mygraph, (void*)1);
    graph_add_node(&mygraph, (void*)2);
    graph_add_node(&mygraph, (void*)4);
    graph_add_node(&mygraph, (void*)3);
    graph_add_node(&mygraph, (void*)5);
    graph_add_node(&mygraph, (void*)6);
    graph_add_edge(&mygraph, (void*)1, (void*)2);
    graph_add_edge(&mygraph, (void*)2, (void*)1);
    graph_add_edge(&mygraph, (void*)2, (void*)3);
    graph_add_edge(&mygraph, (void*)2, (void*)5);
    graph_add_edge(&mygraph, (void*)3, (void*)4);
    graph_add_edge(&mygraph, (void*)3, (void*)6);
    graph_add_edge(&mygraph, (void*)4, (void*)6);

    assert(graph_count_nodes(&mygraph) == 6);
    assert(graph_count_edges(&mygraph) == 7);

    graph_destroy(&mygraph);
    printf("Graph test \"3 - count edges explicitly\" passed!\n");
    return 0;
}
