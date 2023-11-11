#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>

#define __QUEUE_IMPLEMENTATION__
#include "util_queue.h"

#define __HEAP_IMPLEMENTATION__
#include "util_heap.h"

#define __ITER_IMPLEMENTATION__
#include "util_iter.h"


bool no_compare_func(const void *left, const void *right) {
    return true;
}

// If left is smaller then right
bool min_compare_func(const void *left , const void *right) {
    if (*(int *)left < *(int *)right) {
        return true;
    }
    return false;
}

// If left is smaller then right
bool max_compare_func(const void *left , const void *right) {
    if (*(int *)left < *(int *)right) {
        return false;
    }
    return true;
}

int main() {

    int list[] = { 10,5,7,9,20 };
    
    heap_t* heap = _heap_create(4, max_compare_func);
    for(int i = 0; i < 5; i++) {
        int *p = malloc(sizeof(int));
        *p = list[i];
        _heap_append(heap, p);
    }

    heap_iterator_t* iter = _heap_iterator_create(heap);
    while (_heap_iterator_has_next(iter)) {
        int* x = _heap_iterator_next(iter);
        printf("%d\n", *x);
    }
    _heap_delete(iter);

    while((heap->count > 0) == true) {
        int *p = _heap_pop(heap);
        printf("Value: %d\n", *p);
        free(p);
    }

    _heap_delete(heap);

    return 0;
}
