/**
 * Ben Woodhead 
 * 101057746
 * 
 * Emad Alomari
 * 101162157
 * 
 * Fall 2023 Sysc 4001
*/


#ifndef __HARDWARE_H__
#define __HARDWARE_H__

typedef struct {
    unsigned int process_id;
    unsigned int program_counter;
    unsigned int clock;
    unsigned int interrupt;
    unsigned int mdr;
    unsigned int preempt; 
} cpu_t;

typedef struct {
    size_t frag_size;
    void* data;
} memory_frag_t;

typedef struct {
    bool unlimited;
    size_t count;
    size_t blocks;
    heap_t* memory_blocks;
} main_memory_t;

cpu_t* _cpu_create(int preempt);
void _cpu_delete(cpu_t* self);

main_memory_t* _main_memory_create(size_t count, int *memory_list, bool unlimited);
void _main_memory_delete(main_memory_t* self);

bool _main_memory_is_fit_possible(main_memory_t* self, job_t* job);
int _main_memory_append(main_memory_t* self, pcb_t* process);
bool _main_memory_free(main_memory_t* self, pcb_t* process);

bool _main_memory_first_compare_func(const void *left, const void *right);
bool _main_memory_best_compare_func(const void *left, const void *right);
bool _main_memory_worst_compare_func(const void *left, const void *right);


#endif // __HARDWARE_H__

#ifdef __HARDWARE_IMPLEMENTATION__

cpu_t* _cpu_create(int preempt) {
    cpu_t* cpu = malloc(sizeof(cpu_t));
    assert(cpu != NULL);
    cpu->process_id = 0;
    cpu->program_counter = 0;
    cpu->interrupt = 0;
    cpu->clock = 0;
    cpu->mdr = 0;
    cpu->preempt = preempt;
    return cpu;
}

void _cpu_delete(cpu_t* self) {
    assert(self != NULL);
    free(self);
}

void _cpu_clear(cpu_t* self) {
    self->process_id = 0;
    self->program_counter = 0;
    self->interrupt = 0;
    self->mdr = 0;
}

main_memory_t* _main_memory_create(size_t count, int *memory_list, bool unlimited) {
    assert(count > 0);
    assert(memory_list != NULL);

    main_memory_t* memory = malloc(sizeof(main_memory_t));
    assert(memory != NULL);

    if (unlimited == true) {
        memory->unlimited = unlimited;
        return memory;
    }
    
    memory->count = 0;
    memory->blocks = count;
    memory->memory_blocks = _heap_create(count, _main_memory_first_compare_func);

    for (int i = 0; i < count; i++) {
        memory_frag_t* block = malloc(sizeof(memory_frag_t));
        assert(block != NULL);
        block->frag_size = memory_list[i];
        block->data = NULL;
        _heap_append(memory->memory_blocks, block);
    }
    return memory;
}

void _main_memory_delete(main_memory_t* self) {
    assert(self != NULL);
    _heap_delete(self->memory_blocks);
    free(self);
}

bool _main_memory_is_fit_possible(main_memory_t* self, job_t* job) {
    assert(self != NULL);
    assert(job != NULL);

    if (self->unlimited == true) {
        return true;
    }

    heap_iterator_t* iter = _heap_iterator_create(self->memory_blocks);
    while (_heap_iterator_has_next(iter)) {
        memory_frag_t* frag = _heap_iterator_next(iter);
        if (frag->frag_size > job->memory_size) {
            return true;
        }
    }
    _heap_iterator_delete(iter); 
    return false;
}

int _main_memory_append(main_memory_t* self, pcb_t* process) {
    assert(self != NULL);
    assert(process != NULL);

    if (self->unlimited == true) {
        return 0;
    }

    int index = 1; 
    heap_iterator_t* iter = _heap_iterator_create(self->memory_blocks);
    while (_heap_iterator_has_next(iter)) {
        memory_frag_t* frag = _heap_iterator_next(iter);
        if (frag->frag_size > process->memory_size) {
            frag->data = process;
            return index;
        }
        index++;
    }
    _heap_iterator_delete(iter);
    return -1;
}

bool _main_memory_free(main_memory_t* self, pcb_t* process) {
    assert(self != NULL);
    assert(process != NULL);

    if (self->unlimited == true) {
        return 0;
    }

    heap_iterator_t* iter = _heap_iterator_create(self->memory_blocks);
    while (_heap_iterator_has_next(iter)) {
        memory_frag_t* frag = _heap_iterator_next(iter);
        if (frag->data == NULL) {
            continue;
        }
        pcb_t* p = (pcb_t*)frag->data;
        if (p->pid == process->pid) {
            frag->data = NULL;
            return true;
        }
    }
    _heap_iterator_delete(iter);
    return false;
}

bool _main_memory_first_compare_func(const void *left, const void *right) {
    return true;
}

bool _main_memory_best_compare_func(const void *left, const void *right) {
    memory_frag_t* frag_left = (memory_frag_t*)left;
    memory_frag_t* frag_right = (memory_frag_t*)right;
    
    if (frag_left->frag_size > frag_right->frag_size) {
        return false;
    }
    return true;
}

bool _main_memory_worst_compare_func(const void *left, const void *right) {
    memory_frag_t* frag_left = (memory_frag_t*)left;
    memory_frag_t* frag_right = (memory_frag_t*)right;
    
    if (frag_left->frag_size < frag_right->frag_size) {
        return true;
    }
    return false;
}

#endif // __HARDWARE_IMPLEMENTATION__