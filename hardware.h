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
    size_t count;
    size_t backing_size;
    heap_t* memory_blocks;
} main_memory_t;

cpu_t* _cpu_create(int preempt);
void _cpu_delete(cpu_t* self);

main_memory_t* _main_memory_create(size_t count, int *memory_list);
void _main_memory_delete(main_memory_t* self);

bool _main_memory_is_fit_possible(main_memory_t* self, job_t* job);
bool _main_memory_check_availability(main_memory_t* self, pcb_t* process);
bool _main_memory_append(main_memory_t* self, pcb_t* process);
int _main_memory_get_total(main_memory_t* self);
    bool _main_memory_remove(main_memory_t* self, pcb_t* process);

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

main_memory_t* _main_memory_create(size_t count, int *memory_list) {

    main_memory_t* memory = malloc(sizeof(main_memory_t));
    assert(memory != NULL);
    
    memory->count = 0;
    if (count == 0) {
        count = 20;
        memory_list = malloc(count * sizeof(int));
        for(int i = 0; i < count; i++) {
            memory_list[i] = 0;
        }
    }

    memory->backing_size = count;
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

    heap_iterator_t* iter = _heap_iterator_create(self->memory_blocks);
    while (_heap_iterator_has_next(iter)) {
        memory_frag_t* frag = _heap_iterator_next(iter);
        if ((frag->frag_size >= job->memory_size) || (frag->frag_size == 0)) {
            return true;
        }
    }
    _heap_iterator_delete(iter); 
    return false;
}

bool _main_memory_check_availability(main_memory_t* self, pcb_t* process) {
    assert(self != NULL);
    assert(process != NULL);

    heap_iterator_t* iter = _heap_iterator_create(self->memory_blocks);
    while (_heap_iterator_has_next(iter)) {
        memory_frag_t* frag = _heap_iterator_next(iter);
        if (frag->data != NULL) {
            continue;
        }
        if ((frag->frag_size >= process->memory_size) || (frag->frag_size == 0)) {
            return true;
        }
    }
    _heap_iterator_delete(iter);
    return false;
}

bool _main_memory_append(main_memory_t* self, pcb_t* process) {
    assert(self != NULL);
    assert(process != NULL);

    int index = 0; 
    heap_iterator_t* iter = _heap_iterator_create(self->memory_blocks);
    while (_heap_iterator_has_next(iter)) {
        index++;
        memory_frag_t* frag = _heap_iterator_next(iter);
        if (frag->data != NULL) {
            continue;
        }
        if ((frag->frag_size >= process->memory_size) || (frag->frag_size == 0)) {
            frag->data = process;
            process->memory_location = index;
            self->count++;
            int total = _main_memory_get_total(self);
            printf("Accepted: %d Total Memory; %dk\n", process->pid, total);
            return true;
        }
    }
    _heap_iterator_delete(iter);
    return false;
}

int _main_memory_get_total(main_memory_t* self) {

    heap_iterator_t* iter = _heap_iterator_create(self->memory_blocks);
    int total = 0;
    while (_heap_iterator_has_next(iter)) {
        memory_frag_t* frag = _heap_iterator_next(iter);
        if (frag->data == NULL) {
            continue;
        }
        pcb_t* process = (pcb_t*)frag->data;
        total += process->memory_size;
    }            
    return total;
}

bool _main_memory_remove(main_memory_t* self, pcb_t* process) {
    assert(self != NULL);
    assert(process != NULL);

    heap_iterator_t* iter = _heap_iterator_create(self->memory_blocks);
    while (_heap_iterator_has_next(iter)) {
        memory_frag_t* frag = _heap_iterator_next(iter);
        if (frag->data == NULL) {
            continue;
        }
        pcb_t* p = (pcb_t*)frag->data;
        if (p->pid == process->pid) {
            frag->data = NULL;
            int total = _main_memory_get_total(self);
            printf("Removed: %d Total Memory; %dk\n", process->pid, total);
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