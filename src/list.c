#include <stdio.h>
#include <stdlib.h>
#include "list.h"

// initializes the list
void list_init(list_t *this){
    this->front = NULL;
    this->back = NULL;
    this->N = 0;
}

// adds an item to the back of the list
void list_pushback(list_t* this, list_item item) {
    list_node_t *node;

    node = (list_node_t *) malloc(sizeof(list_node_t));
    node->item = item;
    node->next = NULL;

    if (this->back == NULL) {
        this->front = node;
    } else {
        this->back->next = node;
    }

    this->back = node;
    this->N++;
}

// adds an item to the front of the list
void list_pushfront(list_t* this, list_item item){
    list_node_t* node;

    node = (list_node_t*) malloc(sizeof(list_node_t));
    node->item = item;
    node->next = this->front;

    if(this->back == NULL)
        this->back = node;

    this->front = node;
    this->N++;
}

// returns the size of the list (no. of elements in the list)
int list_size(list_t* this){
    return this->N;
}

// checks if the list is empty and returns 1 or 0, depending on the list's status
int list_isempty(list_t *this){
    return this->N == 0;
}

// removes an element from the front of the list
list_item list_popfront(list_t* this){
    list_item ans;
    list_node_t* node;

    if(this->front == NULL){
        fprintf(stderr, "Error: underflow in list_popfront.\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    node = this->front;
    ans = node->item;

    this->front = this->front->next;
    if (this->front == NULL) this->back = NULL;
    free(node);

    this->N--;

    return ans;
}

// returns the element in the front of the list
list_item list_head(list_t* this) {
    if(this->front == NULL){
        fprintf(stderr, "Error: underflow in list_head.\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    return this->front->item;
}

// removes an element from the list
list_item list_remove(list_t* this, list_item item) {
    list_node_t* current = this->front;
    list_item removed_item = NULL;
    list_node_t* prev = NULL;

    if (current == NULL)
        return current;

    while (current != NULL) {
        if (current->item == item) {
            removed_item = current->item;
            if (prev != NULL)
                prev->next = current->next;
            else
                this->front = current->next;

            if (current->next == NULL)
                this->back = prev;

            free(current);
            this->N--;
            break;
        }

        prev = current;
        current = current->next;
    }

    return removed_item;
}

// finds whether an element exists in the list or not
bool list_find(list_t* this, list_item item) {
    if (this->N == 0)
        return false;

    list_node_t* current = this->front;
    while (current != NULL) {
        if (current->item == item)
            return true;

        current = current->next;
    }

    return false;
}

// prints the elements in the list
void print_list_elements(list_t* this) {
    list_node_t* current = this->front;
    printf("==============================\n");
    while (current != NULL) {
        printf("list element: %p\n", current->item);
        current = current->next;
    }
    printf("==============================\n");
}

// frees up all the resources used by the list
void list_destroy(list_t* this) {
    while(!list_isempty(this))
        list_popfront(this);
}
