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
    //unsigned int text_start_address;
    unsigned int total_cpu;
    unsigned int io_frequency;
    unsigned int io_duration;

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

    pcb->remaining_io_cycles = job->io_duration;

    // Create a process control block
    // new_process->pid = accepted_job->pid;
    // new_process->program_counter = 0;
    // new_process->flags = 0;
    // new_process->total_cpu = accepted_job->total_cpu;
    // new_process->text_start_address = main_memory->count;
    // new_process->remaining_io_cycles = 0;

    // //_pcb_print(new_process);
    // //_queue_push(process_queue, new_process);

    // // The start data is after JOB and RUN
    // //size_t data_start = new_process->text_start_address + accepted_job->total_cpu + 2;
    // size_t data_start = accepted_job->total_cpu + 3;

    // // Kernel Mode
    // opcode_t header[] = {JOB|data_start, RUN};
    // _main_memory_append(main_memory, header, 2);

    // // Setup the main program
    // for(unsigned int i = 1; i < accepted_job->total_cpu+1; i++) {
    //     if ((i % accepted_job->io_frequency) == 0 ) {
    //         opcode_t code[] = {READ | data_start+1};
    //          _main_memory_append(main_memory, code, 1);
    //     } else {
    //         opcode_t code[] = {NOP};
    //         _main_memory_append(main_memory, code, 1);
    //     }
    // }
    // opcode_t footer[] = {END|data_start};
    // _main_memory_append(main_memory, footer, 1);

    // unsigned int data[] = {accepted_job->pid, accepted_job->io_duration};
    // _main_memory_append(main_memory, data, 2);
}

void _pcb_print(pcb_t* pcb) {
    printf("----------------------------\n");
    printf("PID: %d\n", pcb->pid);
    printf("Program Counter: %d\n", pcb->program_counter);
    printf("Flags: %d\n", pcb->flags);
    printf("Total CPU: %d\n", pcb->total_cpu);
    printf("Remaining IO Cycles: %d\n", pcb->remaining_io_cycles);  
    printf("----------------------------\n");
}

#endif //__SPOOL_IMPLEMENTATION__