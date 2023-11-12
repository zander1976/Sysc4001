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

void _pbc_admit_job(pcb_t* pcb, job_t* job);
void _pcb_print(pcb_t* pcb);
bool _pcb_fcfs_compare_func(const void *left, const void *right);
bool _pcb_priority_compare_func(const void *left, const void *right);

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


#endif //__SPOOL_IMPLEMENTATION__