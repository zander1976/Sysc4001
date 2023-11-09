#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>

#define __HEAP_IMPLEMENTATION__
#include "util_heap.h"

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

int main() {

    int list[] = { 10,5,7,9,20 };
    
    heap_t* heap = _heap_create(4, min_compare_func);
    for(int i = 0; i < 5; i++) {
        int *p = malloc(sizeof(int));
        *p = list[i];
        _heap_append(heap, p);
    }

    while((heap->count > 0) == true) {
        int *p = _heap_pop(heap);
        printf("Value: %d\n", *p);
        free(p);
    }

    _heap_delete(heap);

    return 0;
}
