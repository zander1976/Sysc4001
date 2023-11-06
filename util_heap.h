/**
 * Ben Woodhead 
 * 101057746
 * 
 * Emad Alomari
 * 101162157
 * 
 * Fall 2023 Sysc 4001
*/
#ifndef __HEAP_H__
#define __HEAP_H__

#include <stdlib.h>
#include <stdbool.h>

// Define the heap_t structure
typedef struct {
    size_t blocksize;
    size_t length;
    void** data;
    int (*compare_func)(const void*, const void*);
} heap_t;

heap_t* _heap_create(size_t blocksize, int (*compare_func)(const void*, const void*));
bool _heap_insert(heap_t* heap, void* element);
bool _heap_remove(heap_t* heap);

#endif // __HEAP_H__

#ifdef __HEAP_IMPLEMENTATION__

// Function to create a new heap
heap_t* _heap_create(size_t blocksize, int (*compare_func)(const void*, const void*)) {
    heap_t* heap = malloc(sizeof(heap_t));
    assert(heap != NULL);

    heap->data = malloc(blocksize * sizeof(void*));
    assert(heap->data != NULL);

    heap->blocksize = blocksize;
    heap->count = 0;
    heap->compare_func = compare_func;

    return heap;
}

bool _heap_insert(heap_t* heap, void* element) {
    if (!heap || !element) {
        return false; 
    }

    // Resize the data array if necessary
    if (heap->count >= heap->blocksize) {
        heap->blocksize *= 2;
        heap->data = realloc(heap->data, heap->blocksize * sizeof(void*));
        assert(heap->data != NULL);
    }

    // Insert the element and adjust the heap
    size_t index = heap->count++;
    while (index > 0) {
        size_t parent = (index - 1) / 2;
        if (heap->compare_func(element, heap->data[parent]) >= 0) {
            break;
        }
        heap->data[index] = heap->data[parent];
        index = parent;
    }
    heap->data[index] = element;

    return true;
}

bool _heap_remove(heap_t* heap) {
    if (!heap || heap->count == 0) {
        return false; 
    }

    // Swap the top element with the last element
    void* top_element = heap->data[0];
    void* last_element = heap->data[heap->count - 1];
    heap->data[0] = last_element;
    heap->count--;

    // Re-establish the heap property (heapify)
    size_t index = 0;
    while (true) {
        size_t left_child = 2 * index + 1;
        size_t right_child = 2 * index + 2;
        if (left_child >= heap->count) {
            break; // No more children
        }
        size_t min_child = left_child;
        if (right_child < heap->count && heap->compare_func(heap->data[right_child], heap->data[left_child]) < 0) {
            min_child = right_child;
        }
        if (heap->compare_func(heap->data[min_child], last_element) >= 0) {
            break; // Heap property is restored
        }
        heap->data[index] = heap->data[min_child];
        index = min_child;
    }
    heap->data[index] = last_element;
    free(top_element);

    return true;
}

#endif //__HEAP_IMPLEMENTATION__
