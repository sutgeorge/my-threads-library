#include <stdio.h>
#include <assert.h>
#include "../src/graph.h"

int main() {
    graph_t mygraph;
    graph_init(&mygraph);

    graph_add_node(&mygraph, (void*)1);
    graph_add_node(&mygraph, (void*)2);
    graph_add_node(&mygraph, (void*)3);
    graph_add_edge(&mygraph, (void*)1, (void*)2);
    graph_add_edge(&mygraph, (void*)2, (void*)1);
    graph_add_edge(&mygraph, (void*)2, (void*)3);

    assert(graph_count_edges(&mygraph) == 3);

    graph_remove_edge(&mygraph, (void*)2, (void*)1);

    assert(graph_count_edges(&mygraph) == 2);

    graph_remove_edge(&mygraph, (void*)2, (void*)1);

    assert(graph_count_edges(&mygraph) == 2);

    graph_remove_edge(&mygraph, (void*)2, (void*)3);

    assert(graph_count_edges(&mygraph) == 1);

    graph_remove_edge(&mygraph, (void*)1, (void*)2);

    assert(graph_count_edges(&mygraph) == 0);

    printf("Graph test \"6 - removing edges\" passed!\n");
    return 0;
}
