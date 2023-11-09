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

int main() {

    queue_t* test = _queue_create();

    for (int i = 0; i < 5; i++) {
        int* x = malloc(sizeof(int));
        *x = i;
        _queue_push(test, x);
    }

    queue_iterator_t* iter = _queue_iterator_create(test);
    while (_queue_iterator_has_next(iter)) {
        int* x = _queue_iterator_next(iter);
        printf("%d\n", *x);
    }
    _queue_iterator_delete(iter);

    while((test->count > 0) == true) {
        int* x = _queue_peak(test);
        int* y = _queue_pop(test);
        if (x != NULL) {
            printf("%d %d\n", *x, *y);
        }
        free(x); // Free the data
    }

    for (int i = 0; i < 3; i++) {
        int* x = malloc(sizeof(int));
        *x = i;
        _queue_push(test, x);
    }

    while((test->count > 0) == true) {
        int* x = _queue_peak(test);
        int* y = _queue_pop(test);
        if (x != NULL) {
            printf("%d %d\n", *x, *y);
        }
        free(x); // Free the data
    }

    for (int i = 0; i < 3; i++) {
        int* x = malloc(sizeof(int));
        *x = i;
        _queue_push(test, x);
    }
    
    _queue_delete(test);

    return 0;
}