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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    unsigned int pid;
    unsigned int program_counter;

    unsigned int flags;
    unsigned int total_cpu;
    unsigned int io_frequency;
    unsigned int io_duration;

    unsigned int wait_time;

    unsigned int remaining_io_cycles;
} pcb_t;

void _pbc_admit_job(pcb_t* pcb, job_t* job);
void _pcb_print(pcb_t* pcb);

#endif //__SPOOL_H__

#ifdef __PCB_IMPLEMENTATION__

void _pbc_admit_job(pcb_t* pcb, job_t* job) {
    assert(pcb != NULL);
    assert(job != NULL);

    // Create a process control block
    pcb->pid = job->pid;
    pcb->program_counter = 0;
    pcb->flags = 0;
    pcb->total_cpu = job->total_cpu;
    pcb->io_frequency = job->io_frequency;
    pcb->io_duration = job->io_duration;

    pcb->wait_time = 0;

    pcb->remaining_io_cycles = job->io_duration;
}

void _pcb_print(pcb_t* pcb) {
    printf("----------------------------\n");
    printf("PID: %d\n", pcb->pid);
    printf("Program Counter: %d\n", pcb->program_counter);
    printf("Flags: %d\n", pcb->flags);
    printf("Total CPU: %d\n", pcb->total_cpu);
    printf("IO Frequency: %d\n", pcb->io_frequency);
    printf("IO Duration: %d\n", pcb->io_duration);
    printf("Total Wait: %d\n", pcb->wait_time);
    printf("Remaining IO Cycles: %d\n", pcb->remaining_io_cycles);  
    printf("----------------------------\n");
}

#endif //__SPOOL_IMPLEMENTATION__