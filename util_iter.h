#ifndef __ITER_H__
#define __ITER_H__

typedef struct {
    heap_t* heap;  
    size_t index; 
} heap_iterator_t;


heap_iterator_t* _heap_iterator_create(heap_t* heap);
bool _heap_iterator_has_next(heap_iterator_t* iterator);
void* _heap_iterator_next(heap_iterator_t* iterator);
void _heap_iterator_delete(heap_iterator_t* iterator);

#endif //__ITER_H__

#ifdef __ITER_IMPLEMENTATION__

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
