#include <stdio.h>
#include <stdlib.h>
#include "dlist.h"

// initializes the dlist
void dlist_init(dlist_t *this){
    this->front = NULL;
    this->back = NULL;
    this->N = 0;
}

// adds an item to the back of the dlist
void dlist_pushback(dlist_t* this, dlist_item item){
    dlist_node_t* node;

    node = (dlist_node_t*) malloc(sizeof(dlist_node_t));
    node->item = item;
    node->next = NULL;

    if(this->back == NULL)
        this->front = node;
    else
        this->back->next = node;

    this->back = node;
    this->N++;
}

// adds an item to the front of the dlist
void dlist_pushfront(dlist_t* this, dlist_item item){
    dlist_node_t* node;

    node = (dlist_node_t*) malloc(sizeof(dlist_node_t));
    node->item = item;
    node->next = this->front;

    if(this->back == NULL)
        this->back = node;

    this->front = node;
    this->N++;
}

// returns the size of the dlist (no. of elements in the dlist)
int dlist_size(dlist_t* this){
    return this->N;
}

// checks if the dlist is empty and returns 1 or 0, depending on the dlist's status
int dlist_isempty(dlist_t *this){
    return this->N == 0;
}

// removes an element from the front of the dlist
dlist_item dlist_popfront(dlist_t* this){
    dlist_item ans;
    dlist_node_t* node;

    if(this->front == NULL){
        fprintf(stderr, "Error: underflow in dlist_popfront.\n");
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

// returns the element in the front of the dlist
dlist_item dlist_head(dlist_t* this) {
    if(this->front == NULL){
        fprintf(stderr, "Error: underflow in dlist_head.\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    return this->front->item;
}

// frees up all the resources used by the dlist
void dlist_destroy(dlist_t* this) {
    while(!dlist_isempty(this))
        dlist_popfront(this);
}
