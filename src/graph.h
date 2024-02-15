#ifndef GRAPH_H
#define GRAPH_H
#include "list.h"

typedef struct {
    list_t nodes; // we should store the thread IDs in this doubly-linked list
    list_t adjacency_list; // we should store the edges between nodes/threads here
} graph_t;

typedef list_item graph_node;

void graph_init(graph_t* this);

void graph_add_node(graph_t* this, graph_node node);

void graph_remove_node(graph_t* this, graph_node node);

void graph_add_edge(graph_t* this, graph_node source_node, graph_node destination_node);

void graph_remove_edge(graph_t* this, graph_node source_node, graph_node destination_node);

int graph_count_nodes(graph_t* this);

int graph_count_edges(graph_t* this);

int graph_print_edges(graph_t* this);

bool graph_dfs(graph_t* this);

bool graph_visit(graph_t* this, graph_node node, list_t* discovered, list_t* finished);

void graph_destroy(graph_t* this);

#endif