#ifndef __ITER_H__
#define __ITER_H__

typedef struct {
    queue_t* queue;  
    node_t* current; 
} queue_iterator_t;

typedef struct {
    heap_t* heap;  
    size_t index; 
} heap_iterator_t;

queue_iterator_t* _queue_iterator_create(queue_t* queue);
bool _queue_iterator_has_next(queue_iterator_t* iterator);
void* _queue_iterator_next(queue_iterator_t* iterator);
void _queue_iterator_delete(queue_iterator_t* iterator);

heap_iterator_t* _heap_iterator_create(heap_t* heap);
bool _heap_iterator_has_next(heap_iterator_t* iterator);
void* _heap_iterator_next(heap_iterator_t* iterator);
void _heap_iterator_delete(heap_iterator_t* iterator);

#endif //__ITER_H__

#ifdef __ITER_IMPLEMENTATION__


queue_iterator_t* _queue_iterator_create(queue_t* queue) {
    queue_iterator_t* iterator = malloc(sizeof(queue_iterator_t));
    assert(iterator);
    iterator->queue = queue;
    iterator->current = queue->head->next; // Start at the first element
    return iterator;
}

bool _queue_iterator_has_next(queue_iterator_t* iterator) {
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

heap_iterator_t* _heap_iterator_create(heap_t* heap) {
    heap_iterator_t* iterator = malloc(sizeof(heap_iterator_t));
    assert(iterator);

    iterator->heap = heap;
    iterator->index = 0;

    return iterator;
}

bool _heap_iterator_has_next(heap_iterator_t* iterator) {
    if (iterator->index >= iterator->heap->count) {
        return false;
    }
    return true;
}

void* _heap_iterator_next(heap_iterator_t* iterator) {
    if (iterator->index >= iterator->heap->count) {
        return NULL;
    }
    void* data = iterator->heap->data[iterator->index];
    iterator->index++;
    return data;
}

void _heap_iterator_delete(heap_iterator_t* iterator) {
    free(iterator);
}

#endif //__ITER_IMPLEMENTATION__
