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

void queue_init(queue_t* this);
int queue_isempty(queue_t* this);
int queue_size(queue_t* this);
void queue_enqueue(queue_t* this, queue_item item);
void queue_push(queue_t* this, queue_item item);
queue_item queue_pop(queue_t* this);
void queue_cycle(queue_t* this);
queue_item queue_front(queue_t* this);
void queue_destroy(queue_t* this);

#endif
