/**
 * Ben Woodhead 
 * 101057746
 * 
 * Emad Alomari
 * 101162157
 * 
 * Fall 2023 Sysc 4001
*/

#ifndef __PCB_H__
#define __PCB_H__

typedef struct {
    unsigned int pid;
    unsigned int program_counter;
    unsigned int total_cpu_time;
    unsigned int io_frequency;
    unsigned int io_duration;
    unsigned int arrival_time;
    unsigned int departed_time;
    unsigned int wait_count;
    unsigned int total_wait_time;
    unsigned int response_count;
    unsigned int total_response_time;
    unsigned int remaining_io_cycles; 
    unsigned int priority;
    unsigned int memory_size;
    unsigned int memory_location;
} pcb_t;

typedef struct {
    size_t count;
    size_t block_count;
    pcb_t** blocks;
} pcb_list_t;

void _pbc_admit_job(pcb_t* pcb, job_t* job);
void _pcb_print(pcb_t* pcb);
bool _pcb_fcfs_compare_func(const void *left, const void *right);
bool _pcb_priority_compare_func(const void *left, const void *right);

pcb_list_t* _pcb_list_create(size_t start_count);
void _pcb_list_append(pcb_list_t* self, void* element);
void* _pcb_list_remove(pcb_list_t* self, size_t index);
void _pcb_list_delete(pcb_list_t* self);

#endif //__PCB_H__

#ifdef __PCB_IMPLEMENTATION__

void _pbc_admit_job(pcb_t* pcb, job_t* job) {
    assert(pcb != NULL);
    assert(job != NULL);

    // Create a process control block
    pcb->pid = job->pid;
    pcb->program_counter = 0;
    pcb->total_cpu_time = job->total_cpu;
    pcb->io_frequency = job->io_frequency;
    pcb->io_duration = job->io_duration;
    pcb->arrival_time = job->arrival_time;
    pcb->departed_time = 0;
    pcb->wait_count = 0;
    pcb->total_wait_time = 0;
    pcb->response_count = 0;
    pcb->total_response_time = 0;
    pcb->remaining_io_cycles = job->io_duration;
    pcb->priority = job->priority;
    pcb->memory_size = job->memory_size;
    pcb->memory_location = 0;
}

bool _pcb_fcfs_compare_func(const void *left, const void *right) {
    return true;
}

// If left is smaller then right
bool _pcb_priority_compare_func(const void *left, const void *right) {
    pcb_t* process_left = (pcb_t*)left;
    pcb_t* process_right = (pcb_t*)right;
    
    if (process_left->priority < process_right->priority) {
        return false;
    }
    return true;
}

pcb_list_t* _pcb_list_create(size_t start_count) {
    pcb_list_t* list = malloc(sizeof(pcb_list_t));
    assert(list != NULL);

    list->count = 0;
    list->block_count = start_count;

    list->blocks = malloc(start_count * sizeof(pcb_t*));
    assert(list->blocks != NULL);
    return list;
}

void _pcb_list_delete(pcb_list_t* self) {
    assert(self != NULL);
    for (size_t i = 0; i < self->count; i++) {
        free(self->blocks[i]);  
    }
    free(self->blocks);  
    free(self);
}

void _pcb_list_append(pcb_list_t* self, void* element) {
    assert(self != NULL);
    assert(element != NULL);
    if (self->count + 1 >= self->block_count) {
        self->block_count *= 2;
        self->blocks = realloc(self->blocks, self->block_count * sizeof(pcb_t*));
        assert(self->blocks != NULL);
    }
    self->blocks[self->count] = element;
    self->count++;
}

void* _pcb_list_remove(pcb_list_t* self, size_t index) {
    assert(self != NULL);
    assert(index < self->count);

    void* data = self->blocks[index];

    for (size_t i = index; i < self->count - 1; i++) {
        self->blocks[i] = self->blocks[i + 1];
    }

    self->count--;

    return data;
}

#endif //__PCB_IMPLEMENTATION__

/*
int main() {
    array_list_t* list = _array_list_create(1, sizeof(int));

    for (int i = 0; i < 5; i++) {
        int* x = malloc(sizeof(int));
        *x = i;
        _array_list_append(list, x);
    }

    int* element = (int*) _array_list_remove(list, 2);
    printf("%d\n", *element);
    free(element);

    while (list->count > 0) {
        int* element = (int*) _array_list_remove(list, 0);
        printf("%d\n", *element);
        free(element);
    }

    _array_list_delete(list);

    return 0;
}
*/
