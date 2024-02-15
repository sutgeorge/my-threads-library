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

void graph_remove_node(graph_t* this, graph_node node) {
    list_node_t* current = this->nodes.front;
    list_node_t* adjacency_list_head = this->adjacency_list.front;

    while (current != NULL) {
        if (current->item == node) {
            list_remove(&this->adjacency_list, adjacency_list_head);
            list_remove(&this->nodes, node);
            return;
        }

        current = current->next;
        adjacency_list_head = adjacency_list_head->next;
    }
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

void graph_remove_edge(graph_t* this, graph_node source_node, graph_node destination_node) {
    list_node_t* nodes_head = this->nodes.front;
    list_node_t* adjacency_list_head = this->adjacency_list.front;

    while (nodes_head != NULL) {
        if (nodes_head->item == source_node)
            break;
        nodes_head = nodes_head->next;
        adjacency_list_head = adjacency_list_head->next;
    }

    if (adjacency_list_head != NULL)
        list_remove(adjacency_list_head->item, destination_node);
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

bool graph_dfs(graph_t* this) {
    list_t* discovered = (list_t*) malloc(sizeof(list_t));
    list_t* finished = (list_t*) malloc(sizeof(list_t));

    list_node_t* nodes_list_head = this->nodes.front;
    list_node_t* adjacency_list_head = this->adjacency_list.front;
    list_node_t* current_node = NULL;

    bool cycle_detected = false;

    while (nodes_list_head != NULL) {
        current_node = nodes_list_head;
        printf("graph dfs debug -> current_node: %p\n", current_node->item);

        if (!list_find(discovered, current_node->item) && !list_find(finished, current_node->item))
            cycle_detected |= graph_visit(this, current_node->item, discovered, finished);//, cycles);

        nodes_list_head = nodes_list_head->next;
        adjacency_list_head = adjacency_list_head->next;
    }

    list_destroy(discovered);
    list_destroy(finished);

    return cycle_detected;
}

bool graph_visit(graph_t* this, graph_node node, list_t* discovered, list_t* finished) { //, list_t* cycles) {
    list_pushback(discovered, node);

    list_node_t* nodes_list_head = this->nodes.front;
    list_node_t* adjacency_list_head = this->adjacency_list.front;

    while (nodes_list_head != NULL && nodes_list_head->item != node) {
        nodes_list_head = nodes_list_head->next;
        adjacency_list_head = adjacency_list_head->next;
    }

    list_t* adjacency_list = adjacency_list_head->item;
    list_node_t* neighbour_node = adjacency_list->front;

    bool cycle_detected = false;

    while (neighbour_node != NULL) {
        if (list_find(discovered, neighbour_node->item)) {
            cycle_detected = true;

            printf("start of the cycle: %p\n", neighbour_node->item);
            printf("discovered cycle: \n");

            list_node_t* discovered_node = discovered->front;
            list_t* detected_cycle_path = (list_t*) malloc(sizeof(list_t));
            bool cycle_path_started = false;

            while (discovered_node != NULL) {
                if (discovered_node->item == neighbour_node->item)
                    cycle_path_started = true;

                if (cycle_path_started)
                    list_pushback(detected_cycle_path, discovered_node->item);

                discovered_node = discovered_node->next;
            }

            list_pushback(detected_cycle_path, neighbour_node->item);
            print_list_elements(detected_cycle_path);
            break;
        }

        if (!list_find(finished, neighbour_node->item))
            cycle_detected = graph_visit(this, neighbour_node->item, discovered, finished);

        neighbour_node = neighbour_node->next;
    }

    list_remove(discovered, node);
    list_pushback(finished, node);

    return cycle_detected;
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