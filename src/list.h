#ifndef LIST_H
#define LIST_H

typedef void* list_item;

typedef struct list_node_t{
    list_item item;
    struct list_node_t* next;
} list_node_t;

typedef struct {
    list_node_t* front;
    list_node_t* back;
    int N;
} list_t;

// initializes the list
void list_init(list_t* this);

// checks if the list is empty and returns 1 or 0, depending on the list's status
int list_isempty(list_t* this);

// returns the size of the list (no. of elements in the list)
int list_size(list_t* this);

// adds an item to the back of the list
void list_pushback(list_t* this, list_item item);

// adds an item to the front of the list
void list_pushfront(list_t* this, list_item item);

// removes an element from the front of the list
list_item list_popfront(list_t* this);

// returns the element in the front of the list
list_item list_head(list_t* this);

// removes an element from the list
list_item list_remove(list_t* this, list_item item);

void print_list_elements(list_t* this);

// frees up all the resources used by the list
void list_destroy(list_t* this);

#endif
