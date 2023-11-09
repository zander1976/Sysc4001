/**
 * Ben Woodhead 
 * 101057746
 * 
 * Emad Alomari
 * 101162157
 * 
 * Fall 2023 Sysc 4001
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdbool.h>

#ifndef __PCB_H__
#define __PCB_H__

typedef struct {
    unsigned int pid;
    unsigned int program_counter;
    unsigned int total_cpu;
    unsigned int io_frequency;
    unsigned int io_duration;
    unsigned int wait_time;
    unsigned int remaining_io_cycles;
    unsigned int priority;
} pcb_t;

void _pbc_admit_job(pcb_t* pcb, job_t* job);
void _pcb_print(pcb_t* pcb);
bool _pcb_fcfs_compare_func(const void *left, const void *right);
bool _pcb_priority_compare_func(const void *left, const void *right);

#endif //__SPOOL_H__

#ifdef __PCB_IMPLEMENTATION__

void _pbc_admit_job(pcb_t* pcb, job_t* job) {
    assert(pcb != NULL);
    assert(job != NULL);

    // Create a process control block
    pcb->pid = job->pid;
    pcb->program_counter = 0;
    pcb->total_cpu = job->total_cpu;
    pcb->io_frequency = job->io_frequency;
    pcb->io_duration = job->io_duration;
    pcb->wait_time = 0;
    pcb->remaining_io_cycles = job->io_duration;
    pcb->priority = job->priority;
}

void _pcb_print(pcb_t* pcb) {
    printf("----------------------------\n");
    printf("PID: %d\n", pcb->pid);
    printf("Program Counter: %d\n", pcb->program_counter);
    printf("Total CPU: %d\n", pcb->total_cpu);
    printf("IO Frequency: %d\n", pcb->io_frequency);
    printf("IO Duration: %d\n", pcb->io_duration);
    printf("Total Wait: %d\n", pcb->wait_time);
    printf("Remaining IO Cycles: %d\n", pcb->remaining_io_cycles);  
    printf("Priority: %d\n", pcb->priority);  
    printf("----------------------------\n");
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