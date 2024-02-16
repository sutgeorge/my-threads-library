#ifndef QUEUE_H
#define QUEUE_H

typedef void* queue_item;

typedef struct queue_node_t{
    queue_item item;
    struct queue_node_t* next;
} queue_node_t;

typedef struct {
    queue_node_t* front;
    queue_node_t* back;
    int N;
}queue_t;

// initializes the queue
void queue_init(queue_t* this);

// checks if the queue is empty and returns 1 or 0, depending on the queue's status
int queue_isempty(queue_t* this);

// returns the size of the queue (no. of elements in the queue)
int queue_size(queue_t* this);

// adds an item to the back of the queue
void queue_enqueue(queue_t* this, queue_item item);

// adds an item to the front of the queue
void queue_push(queue_t* this, queue_item item);

// removes an element from the front of the queue
queue_item queue_pop(queue_t* this);

// returns the element in the front of the queue
queue_item queue_front(queue_t* this);

// returns the element in the back of the queue
queue_item queue_back(queue_t* this);

// frees up all the resources used by the queue
void queue_destroy(queue_t* this);

#endif
