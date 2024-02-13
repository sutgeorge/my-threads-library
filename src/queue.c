#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// initializes the queue
void queue_init(queue_t *this){
    this->front = NULL;
    this->back = NULL;
    this->N = 0;
}

// adds an item to the back of the queue
void queue_enqueue(queue_t* this, queue_item item){
    queue_node_t* node;

    node = (queue_node_t*) malloc(sizeof(queue_node_t));
    node->item = item;
    node->next = NULL;

    if(this->back == NULL)
        this->front = node;
    else
        this->back->next = node;

    this->back = node;
    this->N++;
}

// adds an item to the front of the queue
void queue_push(queue_t* this, queue_item item){
    queue_node_t* node;

    node = (queue_node_t*) malloc(sizeof(queue_node_t));
    node->item = item;
    node->next = this->front;

    if(this->back == NULL)
        this->back = node;

    this->front = node;
    this->N++;
}

// returns the size of the queue (no. of elements in the queue)
int queue_size(queue_t* this){
    return this->N;
}

// checks if the queue is empty and returns 1 or 0, depending on the queue's status
int queue_isempty(queue_t *this){
    return this->N == 0;
}

// removes an element from the front of the queue
queue_item queue_pop(queue_t* this){
    queue_item ans;
    queue_node_t* node;

    if(this->front == NULL){
        fprintf(stderr, "Error: underflow in queue_pop.\n");
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

// returns the element in the front of the queue
queue_item queue_front(queue_t* this) {
    if(this->front == NULL){
        fprintf(stderr, "Error: underflow in queue_front.\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    return this->front->item;
}

// frees up all the resources used by the queue
void queue_destroy(queue_t* this) {
    while(!queue_isempty(this))
        queue_pop(this);
}
