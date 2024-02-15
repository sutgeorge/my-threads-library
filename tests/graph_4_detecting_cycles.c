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
    graph_add_edge(&mygraph, (void*)2, (void*)3);
    graph_add_edge(&mygraph, (void*)3, (void*)1);

    assert(graph_dfs(&mygraph) == true);

    graph_t mygraph2;
    graph_init(&mygraph2);

    graph_add_node(&mygraph2, (void*)1);
    graph_add_node(&mygraph2, (void*)2);
    graph_add_node(&mygraph2, (void*)3);
    graph_add_node(&mygraph2, (void*)4);
    graph_add_node(&mygraph2, (void*)5);
    graph_add_edge(&mygraph2, (void*)1, (void*)2);
    graph_add_edge(&mygraph2, (void*)2, (void*)3);
    graph_add_edge(&mygraph2, (void*)3, (void*)4);
    graph_add_edge(&mygraph2, (void*)5, (void*)4);
    graph_add_edge(&mygraph2, (void*)5, (void*)2);
    graph_add_edge(&mygraph2, (void*)4, (void*)2);

    assert(graph_dfs(&mygraph2) == true);

    graph_t mygraph3;
    graph_init(&mygraph3);

    graph_add_node(&mygraph3, (void*)1);
    graph_add_node(&mygraph3, (void*)2);
    graph_add_node(&mygraph3, (void*)3);
    graph_add_node(&mygraph3, (void*)4);
    graph_add_node(&mygraph3, (void*)5);
    graph_add_edge(&mygraph3, (void*)1, (void*)2);
    graph_add_edge(&mygraph3, (void*)2, (void*)3);
    graph_add_edge(&mygraph3, (void*)3, (void*)4);
    graph_add_edge(&mygraph3, (void*)5, (void*)4);
    graph_add_edge(&mygraph3, (void*)5, (void*)2);

    graph_print_edges(&mygraph3);
    assert(graph_dfs(&mygraph3) == false);

    graph_t mygraph4;
    graph_init(&mygraph4);

    graph_add_node(&mygraph4, (void*)1);
    graph_add_node(&mygraph4, (void*)2);
    graph_add_node(&mygraph4, (void*)3);
    graph_add_node(&mygraph4, (void*)4);
    graph_add_node(&mygraph4, (void*)5);
    graph_add_edge(&mygraph4, (void*)1, (void*)2);
    graph_add_edge(&mygraph4, (void*)2, (void*)3);
    graph_add_edge(&mygraph4, (void*)3, (void*)4);
    graph_add_edge(&mygraph4, (void*)5, (void*)4);
    graph_add_edge(&mygraph4, (void*)4, (void*)2);
    graph_add_edge(&mygraph4, (void*)4, (void*)3);

    graph_print_edges(&mygraph4);
    assert(graph_dfs(&mygraph4) == true);

    graph_destroy(&mygraph);
    graph_destroy(&mygraph2);
    graph_destroy(&mygraph3);
    graph_destroy(&mygraph4);
    printf("Graph test \"4 - detecting cycles\" passed!\n");
    return 0;
}
