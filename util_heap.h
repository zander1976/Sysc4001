/**
 * Ben Woodhead 
 * 101057746
 * 
 * Emad
 * Fall 2023 Sysc 4001
*/

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#ifndef __HEAP_H__
#define __HEAP_H__

// Define the heap_t structure
typedef struct {
    size_t blocksize;
    size_t count;
    void** data;
    bool (*compare_func)(const void*, const void*);
} heap_t;

heap_t* _heap_create(size_t blocksize, bool (*compare_func)(const void*, const void*));
void _heap_delete(heap_t* self);
bool _heap_append(heap_t* self, void* element);
void* _heap_peak(heap_t* self);
void* _heap_pop(heap_t* self);
void _heap_swap(heap_t* self, const size_t left, const size_t right);



#endif // __HEAP_H__

#ifdef __HEAP_IMPLEMENTATION__

heap_t* _heap_create(size_t blocksize, bool (*compare_func)(const void*, const void*)) {
    heap_t* heap = malloc(sizeof(heap_t));
    assert(heap != NULL);

    heap->data = malloc(blocksize * sizeof(void*));
    assert(heap->data != NULL);

    heap->blocksize = blocksize;
    heap->count = 0;
    heap->compare_func = compare_func;

    return heap;
}

void _heap_delete(heap_t* self) {
    assert(self != NULL);
    while((self->count > 0) == true) {
        void *p = _heap_pop(self);
        free(p);
    }
    free(self);
}

bool _heap_append(heap_t* self, void* element) {
    assert(self != NULL);
    assert(element != NULL);
    
    // Resize the data array if necessary
    if (self->count >= self->blocksize) {
        self->blocksize *= 2;
        self->data = realloc(self->data, self->blocksize * sizeof(void*));
        assert(self->data != NULL);
    }

    // Insert the element
    self->data[self->count] = element;
    size_t index = self->count++;
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (self->compare_func(self->data[parent],element) == true) {
            break;
        }
        
        _heap_swap(self, parent, index);
        //self->data[index] = self->data[parent];
        //self->data[parent] = element;
        index = parent;
    }
    return true;
}

void* _heap_peak(heap_t* self) {
    assert(self != NULL);
    if (self->count <= 0) {
        return false; 
    }
    return self->data[0];
}

void* _heap_pop(heap_t* self) {
    assert(self != NULL);
    if (self->count <= 0) {
        return false; 
    }

    // Swap the top element with the last element
    self->count--;
    void* first_element = self->data[0];
    void* last_element = self->data[self->count];
    self->data[0] = last_element;
    self->data[self->count] = 0;

    size_t index = 0;
    while (true) {
        size_t left_child = 2 * index + 1;
        size_t right_child = 2 * index + 2;

        // If there is no left child then we are the leaf
        if (left_child >= self->count) {
            break;
        }

        // Going left
        if ((right_child >= self->count) || (self->compare_func(self->data[left_child], self->data[right_child]) == true)) {
            if (self->compare_func(self->data[left_child], self->data[index]) == true) {
                _heap_swap(self, index, left_child);
                index = left_child;
            } else {
                break;
            }
        } else { // Going right
            if (self->compare_func(self->data[right_child], self->data[index]) == true) {
                _heap_swap(self, index, right_child);
                index = right_child;
            } else {
                break;
            }
        }
    }

    return first_element;
}

void _heap_swap(heap_t* self, const size_t left, const size_t right) {
    assert(self != NULL);
    if ((left >= self->count) || (right >= self->count)) {
        return; 
    }
    void *temp = self->data[right];
    self->data[right] = self->data[left];
    self->data[left] = temp;        
}

#endif //__HEAP_IMPLEMENTATION__
