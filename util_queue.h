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
    node_t* previous;
    void* data;
};

typedef struct {
    node_t* head;
    size_t count;
} queue_t;

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

    // create the head node
    self->head = malloc(sizeof(node_t));
    assert(self->head != NULL);

    // Setup the basic parameters
    self->count = 0;
    self->head->next = self->head;
    self->head->previous = self->head;
    self->head->data = NULL;

    return self;
}

void* _queue_peak(queue_t* self) {
    if (self->count <= 0) {
        return NULL;
    }
    
    return self->head->previous->data;
}

void _queue_push(queue_t* self, void* data) {
    // Create the node
    node_t* new_data = malloc(sizeof(node_t));
    assert(new_data != NULL);

    // Setup the data
    new_data->data = data;
    self->count++;

    new_data->next = self->head->next;
    new_data->previous = self->head;
    self->head->next = new_data;
    new_data->next->previous = new_data;
}

void* _queue_pop(queue_t* self) {
    if (self->count <= 0) {
        return NULL;
    }
    node_t* popped_data = self->head->previous;
    self->head->previous = popped_data->previous;
    popped_data->previous->next = self->head;
    self->count--;

    void* data = popped_data->data;
    free(popped_data); // Free the popped node

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

#endif //__QUEUE_IMPLEMENTATION__

