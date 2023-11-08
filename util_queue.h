/**
 * Ben Woodhead 
 * 101057746
 * 
 * Emad Alomari
 * 101162157
 * 
 * Fall 2023 Sysc 4001
*/
#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

typedef struct node node_t;
struct node {
    node_t* next;
    void* data;
};

typedef struct {
    node_t* head;
    size_t count;
} queue_t;

typedef struct {
    queue_t* queue;  
    node_t* current; 
} queue_iterator_t;

queue_t* _queue_create();
void* _queue_peak(queue_t* self);
void _queue_push(queue_t* self, void* data);
void* _queue_pop(queue_t* self);
void _queue_delete(queue_t* self);

#endif //__QUEUE_H__

#ifdef __QUEUE_IMPLEMENTATION__

queue_t* _queue_create() {
    // Create the queue
    queue_t* self = malloc(sizeof(queue_t));
    assert(self != NULL);
    
    // Create the head nodes on the heap
    self->head = malloc(sizeof(node_t));
    assert(self->head != NULL);

    self->count = 0;
    self->head->next = NULL;
    self->head->data = NULL;
    
    // Setup the basic parameters
    return self;
}

void* _queue_peak(queue_t* self) {
    if (self->count <= 0) {
        return NULL;
    }
    
    return self->head->next->data;
}

void _queue_push(queue_t* self, void* data) {
    // Create the node
    node_t* new_data = malloc(sizeof(node_t));
    assert(new_data != NULL);

    // Setup the data
    new_data->next = NULL;
    new_data->data = data;
    
    node_t* current = self->head;
    for (; current->next != NULL; current = current->next);

    current->next = new_data;    

    self->count++;
}

void* _queue_pop(queue_t* self) {
    if (self->count <= 0) {
        return NULL;
    }

    node_t* popped_data = self->head->next;
    self->head->next = popped_data->next;

    void* data = popped_data->data;
    free(popped_data); // Free the popped node

    self->count--;

    return data;
}


void _queue_delete(queue_t* self) {
    for (size_t i = 0; i < self->count; i++) {
        void* p = _queue_pop(self);
        free(p);
    }
    free(self->head);
    free(self);
}

queue_iterator_t* _queue_iterator_create(queue_t* queue) {
    queue_iterator_t* iterator = malloc(sizeof(queue_iterator_t));
    if (iterator != NULL) {
        iterator->queue = queue;
        iterator->current = queue->head->next; // Start at the first element
    }
    return iterator;
}

int _queue_iterator_has_next(queue_iterator_t* iterator) {
    return iterator->current != NULL;
}

void* _queue_iterator_next(queue_iterator_t* iterator) {
    if (iterator->current != NULL) {
        void* data = iterator->current->data;
        iterator->current = iterator->current->next;
        return data;
    }
    return NULL;
}

void _queue_iterator_delete(queue_iterator_t* iterator) {
    free(iterator);
}


#endif //__QUEUE_IMPLEMENTATION__

