#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void queue_init(queue_t *this){
    this->front = NULL;
    this->back = NULL;
    this->N = 0;
}

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

int queue_size(queue_t* this){
    return this->N;
}

int queue_isempty(queue_t *this){
    return this->N == 0;
}

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

void queue_cycle(queue_t* this) {
    if(this->back == NULL)
        return;

    this->back->next = this->front;
    this->back = this->front;
    this->front = this->front->next;
    this->back->next = NULL;
}

queue_item queue_front(queue_t* this) {
    if(this->front == NULL){
        fprintf(stderr, "Error: underflow in queue_front.\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    return this->front->item;
}

void queue_destroy(queue_t* this) {
    while(!queue_isempty(this))
        queue_pop(this);
}
