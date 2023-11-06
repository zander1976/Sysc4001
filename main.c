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
#include <assert.h>

#define __QUEUE_IMPLEMENTATION__
#include "util_queue.h"

#define __JOB_SPOOL_IMPLEMENTATION__
#include "job_spool.h"

#define __HARDWARE_IMPLEMENTATION__
#include "hardware.h"

#define __PCB_IMPLEMENTATION__
#include "pcb.h"

#define __STATE_MACHINE_IMPLEMENTATION__
#include "state_machine.h"


// OS has released the resources
void terminate_callback(state_machine_t* self) {
    assert(self != NULL);
    job_t* job = NULL; 
    while( (job = _queue_pop(self->term_queue)) != NULL) {
        // Announce it's deletion
        printf("%u\t%u\t%s\t%s\n", self->cpu->clock, job->pid, "Running", "Terminated");
    }
}

// IOP hascompleted a task
void io_complete_callback(state_machine_t* self) {
    assert(self != NULL);
    pcb_t* pcb = _queue_pop(self->wait_queue);
    if (pcb == NULL) {
        return;
    }
    printf("%u\t%u\t%s\t%s\n", self->cpu->clock, pcb->pid, "Waiting", "Ready");
    _queue_push(self->read_queue, pcb);
    interrupt(self, ST_SCHEDULER);
}

// (JOB Scheduler) timeout for the job or admitted scheduler 
void lt_fcfs_scheduler_callback(state_machine_t* self) {
    assert(self != NULL);
    job_t* job = _queue_peak(self->job_spool);
    if (job == NULL) {
        return;
    }
    if (job->arrival_time <= self->cpu->clock) {
        interrupt(self, ADMITTED);
    }
}

// (JOB Scheduler) timeout for the job or admitted scheduler 
void lt_rr_scheduler_callback(state_machine_t* self) {
    assert(self != NULL);
    job_t* job = _queue_peak(self->job_spool);
    if (job == NULL) {
        return;
    }
    if (job->arrival_time <= self->cpu->clock) {
        interrupt(self, ADMITTED);
    }
}
// (JOB Scheduler) timeout for the job or admitted scheduler 
void lt_multi_scheduler_callback(state_machine_t* self) {
    assert(self != NULL);
    job_t* job = _queue_peak(self->job_spool);
    if (job == NULL) {
        return;
    }
    if (job->arrival_time <= self->cpu->clock) {
        interrupt(self, ADMITTED);
    }
}
// Clock pulse
void clock_pulse_callback(state_machine_t* self) {
    assert(self != NULL);
    self->cpu->clock++;
    self->cpu->program_counter++;
    if (self->running != NULL) {
        if (self->cpu->program_counter == self->running->total_cpu) {
            interrupt(self, SYSCALL_EXIT_REQUEST);
        } 
    }
    if (self->running != NULL) {
        // Check to see if it even has any io
        if (self->running->io_frequency == 0) {
            return;
        }
        // Check to see if the current counter has hit an io timeout
        if ((self->cpu->program_counter % self->running->io_frequency) == 0) {
            interrupt(self, SYSCALL_IO_REQUEST);
        } 
    }
}

// Called from LT Schedulaer when a job is being admitted (move new -> ready)
void admitted_callback(state_machine_t* self) {
    assert(self != NULL);

    job_t* job = _queue_pop(self->job_spool);
    pcb_t* pcb = malloc(sizeof(pcb_t));
    if (pcb == NULL) {
        return;
    }

    _pbc_admit_job(pcb, job);

    printf("%u\t%u\t%s\t%s\n", self->cpu->clock, job->pid, "New", "Ready");

    _queue_push(self->read_queue, pcb);

    interrupt(self, ST_SCHEDULER);
}

// (CPU Scheduler) Selects the next process to run (orders the ready queue or has a variable to hold the next) 
void st_scheduler_callback(state_machine_t* self) {
    assert(self != NULL);

    if (self->running != NULL) {
        return;
    }
    
    // FIFO (just take the first)
    interrupt(self, DISPATCH);
}

// Kernel Mode Context switch and move the process into running (Ready to Running) 
void dispatch_callback(state_machine_t* self) {
    assert(self != NULL);

    pcb_t* pcb = _queue_pop(self->read_queue);
    if (pcb == NULL) {
        return;
    }
    self->running = pcb;
    load_context(self);
    printf("%u\t%u\t%s\t%s\n", self->cpu->clock, self->running->pid, "Ready", "Running");
}

// Kick process out of CPU
void preempt_callback(state_machine_t* self) {
    printf("preempt_callback!\n");
    assert(self != NULL);
}

// System call from User space to do an IO request. Wait time is in cpu->mdr)
void syscall_io_request_callback(state_machine_t* self) {
    assert(self != NULL);

    self->running->remaining_io_cycles = self->running->io_duration;
    printf("%u\t%u\t%s\t%s\n", self->cpu->clock, self->running->pid, "Running", "Waiting");
    save_context(self);
    _queue_push(self->wait_queue, self->running);
    self->running = NULL;
    interrupt(self,ST_SCHEDULER);
}

// System call to End. Exit code is in cpu->mdr
void syscall_exit_request_callback(state_machine_t* self) {

    assert(self != NULL);
    //save_context(self);
    _queue_push(self->term_queue, self->running);
    self->running = NULL;
    interrupt(self,ST_SCHEDULER);
}

int main(int argc, char *argv[]) {

    char* file = NULL;
    char* schedule_type = NULL;
    if (argc != 3) {
        printf("Please run main <file> <schedule type>\n");
	    printf("<file> would be test_case_1.csv\n");
	    printf("<schedule type> would be FCFS, RR or Multi\n");
        file = "test_part_1.csv";
        schedule_type = "FCFS";
        //file = "test_case_1.csv";
	    //return -1;
    } else {
        file = argv[1];
        schedule_type = argv[2];
    }

    // Create the CPU and state machine
    state_machine_t* machine = _state_machine_create();
    _job_load_from_file(file, machine->job_spool);

    // Register all the interupt service request handlers
    _state_machine_register_isr(machine, IRQ_TERMINATED, terminate_callback);
    _state_machine_register_isr(machine, IRQ_IO_COMPLETE, io_complete_callback);

    if (strcmp(schedule_type, "FCFS")) {
        _state_machine_register_isr(machine, IRQ_LT_SCHEDULER_TIMEOUT, lt_fcfs_scheduler_callback);
    } else if (strcmp(schedule_type, "RR")) {
        _state_machine_register_isr(machine, IRQ_LT_SCHEDULER_TIMEOUT, lt_rr_scheduler_callback);
    } else if (strcmp(schedule_type, "Multi")) {
        _state_machine_register_isr(machine, IRQ_LT_SCHEDULER_TIMEOUT, lt_multi_scheduler_callback);
    } else {
        printf("Not a valid scheduler.");
        return 0;
    }

    _state_machine_register_isr(machine, IRQ_CLOCK_PULSE, clock_pulse_callback);
    _state_machine_register_isr(machine, ADMITTED, admitted_callback);
    _state_machine_register_isr(machine, ST_SCHEDULER, st_scheduler_callback);
    _state_machine_register_isr(machine, DISPATCH, dispatch_callback);
    _state_machine_register_isr(machine, PREEMPT, preempt_callback);
    _state_machine_register_isr(machine, SYSCALL_IO_REQUEST, syscall_io_request_callback);
    _state_machine_register_isr(machine, SYSCALL_EXIT_REQUEST, syscall_exit_request_callback);

    // Main event loop
    for(unsigned int i = 0; i < 10000; i++) {

        // Release any terminated processes
        if (machine->term_queue->count != 0) {
            // Call terminated
            interrupt(machine, IRQ_TERMINATED);
        }

        // We can select how often we want to run this
        if ((machine->cpu->clock % 1) == 0) {
            interrupt(machine, IRQ_LT_SCHEDULER_TIMEOUT);
        }
        
        // Check the wait queue
        if (machine->wait_queue != 0) {
            pcb_t* pcb = _queue_peak(machine->wait_queue);
            if (pcb != NULL) {
                pcb->remaining_io_cycles--;
                if (pcb->remaining_io_cycles == 0) {
                    interrupt(machine, IRQ_IO_COMPLETE);
                }
            }
        }

        // Generate a clock pulse
        interrupt(machine,IRQ_CLOCK_PULSE);
    }

    return 0;
 }
