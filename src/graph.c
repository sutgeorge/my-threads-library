#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

void graph_init(graph_t* this) {
    list_init(&this->nodes);
    list_init(&this->adjacency_list);
}

void graph_add_node(graph_t* this, graph_node node) {
    list_pushback(&this->nodes, node);
    list_t* neighbours_list = (list_t*) malloc(sizeof(list_t));
    list_pushback(&this->adjacency_list, neighbours_list);
}

void graph_add_edge(graph_t* this, graph_node source_node, graph_node destination_node) {
    list_node_t* nodes_head = this->nodes.front;
    list_node_t* adjacency_list_head = this->adjacency_list.front;

    while (nodes_head != NULL) {
        if (nodes_head->item == source_node)
            break;
        nodes_head = nodes_head->next;
        adjacency_list_head = adjacency_list_head->next;
    }

    if (adjacency_list_head != NULL)
        list_pushback(adjacency_list_head->item, destination_node);
}

int graph_count_edges(graph_t* this) {
    list_node_t* nodes_head = this->nodes.front;
    list_node_t* adjacency_list_head = this->adjacency_list.front;
    int number_of_edges = 0;

    while (nodes_head != NULL) {
        list_t* neighbours = adjacency_list_head->item;
        list_node_t* neighbours_list_head = neighbours->front;

        while (neighbours_list_head != NULL) {
            neighbours_list_head = neighbours_list_head->next;
            number_of_edges++;
        }

        nodes_head = nodes_head->next;
        adjacency_list_head = adjacency_list_head->next;
    }

    return number_of_edges;
}

int graph_print_edges(graph_t* this) {
    list_node_t* nodes_head = this->nodes.front;
    list_node_t* adjacency_list_head = this->adjacency_list.front;
    int number_of_edges = 0;

    while (nodes_head != NULL) {
        list_t* neighbours = adjacency_list_head->item;
        list_node_t* neighbours_list_head = neighbours->front;

        while (neighbours_list_head != NULL) {
            number_of_edges++;
            printf("Edge no. %d: (%p, %p)\n", number_of_edges, nodes_head->item, neighbours_list_head->item);
            neighbours_list_head = neighbours_list_head->next;
        }

        nodes_head = nodes_head->next;
        adjacency_list_head = adjacency_list_head->next;
    }

    return number_of_edges;
}

int graph_count_nodes(graph_t* this) {
    list_node_t* nodes_head = this->nodes.front;
    int number_of_nodes = 0;

    while (nodes_head != NULL) {
        number_of_nodes++;
        nodes_head = nodes_head->next;
    }

    return number_of_nodes;
}

void graph_destroy(graph_t* this) {
    list_node_t* adjacency_list_head = this->adjacency_list.front;

    list_destroy(&this->nodes);

    while (adjacency_list_head->next != NULL) {
        list_node_t* next_adjacency_list = adjacency_list_head->next;
        list_destroy((list_t*)adjacency_list_head);
        adjacency_list_head = next_adjacency_list;
    }
}