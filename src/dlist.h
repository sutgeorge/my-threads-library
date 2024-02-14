#ifndef DLIST_H
#define DLIST_H

typedef void* dlist_item;

typedef struct dlist_node_t{
    dlist_item item;
    struct dlist_node_t* next;
} dlist_node_t;

typedef struct {
    dlist_node_t* front;
    dlist_node_t* back;
    int N;
}dlist_t;

// initializes the dlist
void dlist_init(dlist_t* this);

// checks if the dlist is empty and returns 1 or 0, depending on the dlist's status
int dlist_isempty(dlist_t* this);

// returns the size of the dlist (no. of elements in the dlist)
int dlist_size(dlist_t* this);

// adds an item to the back of the dlist
void dlist_pushback(dlist_t* this, dlist_item item);

// adds an item to the front of the dlist
void dlist_pushfront(dlist_t* this, dlist_item item);

// removes an element from the front of the dlist
dlist_item dlist_popfront(dlist_t* this);

// returns the element in the front of the dlist
dlist_item dlist_head(dlist_t* this);

// frees up all the resources used by the dlist
void dlist_destroy(dlist_t* this);

#endif
