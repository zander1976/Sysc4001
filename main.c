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
#include <stdarg.h>
#include <unistd.h>

#define __HEAP_IMPLEMENTATION__
#include "util_heap.h"

#define __ITER_IMPLEMENTATION__
#include "util_iter.h"

#define __JOB_SPOOL_IMPLEMENTATION__
#include "job_spool.h"

#define __HARDWARE_IMPLEMENTATION__
#include "hardware.h"

#define __PCB_IMPLEMENTATION__
#include "pcb.h"

#define __STATE_MACHINE_IMPLEMENTATION__
#include "state_machine.h"

void show_state(state_machine_t* machine);
 
// OS has released the resources
void terminate_callback(state_machine_t* self) {
    assert(self != NULL);
    pcb_t* process = NULL; 
    while( (process = _heap_pop(self->term_queue)) != NULL) {
        // Announce it's deletion
        //_pcb_print(process);
        _heap_append(self->report_queue, process);
        printf("%u\t%u\t%s\t%s\n", self->cpu->clock, process->pid, "Running", "Terminated");
        show_state(self);
    }
}

// IOP hascompleted a task
void io_complete_callback(state_machine_t* self) {
    assert(self != NULL);
    pcb_t* pcb = _heap_pop(self->wait_queue);
    if (pcb == NULL) {
        return;
    }
    printf("%u\t%u\t%s\t%s\n", self->cpu->clock, pcb->pid, "Waiting", "Ready");
    show_state(self);
    _heap_append(self->ready_queue, pcb);
    interrupt(self, ST_SCHEDULER);
}

// (JOB Scheduler) timeout for the job or admitted scheduler 
void lt_scheduler_callback(state_machine_t* self) {
    assert(self != NULL);
    job_t* job = _heap_peak(self->new_queue);
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

    // Increment the wait counter
    heap_iterator_t* iter = _heap_iterator_create(self->ready_queue);
    while (_heap_iterator_has_next(iter)) {
        pcb_t* process = _heap_iterator_next(iter);
        process->wait_time += 1;
    }

    if (self->running != NULL) {
        if (self->cpu->program_counter == self->running->total_cpu) {
            interrupt(self, SYSCALL_EXIT_REQUEST);
        } 
    }
    if (self->running != NULL) {
        // Check to see if it even has any io
        if (self->running->io_frequency != 0) {
            // Check to see if the current counter has hit an io timeout
            if ((self->cpu->program_counter % self->running->io_frequency) == 0) {
                interrupt(self, SYSCALL_IO_REQUEST);
            } 
        }
    }
}

// Called from LT Schedulaer when a job is being admitted (move new -> ready)
void admitted_callback(state_machine_t* self) {
    assert(self != NULL);
    show_state(self);

    job_t* job = _heap_pop(self->new_queue);
    pcb_t* process = malloc(sizeof(pcb_t));
    if (process == NULL) {
        return;
    }

    _pbc_admit_job(process, job);

    printf("%u\t%u\t%s\t%s\n", self->cpu->clock, job->pid, "New", "Ready");

    _heap_append(self->ready_queue, process);

    show_state(self);

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

    pcb_t* pcb = _heap_pop(self->ready_queue);
    if (pcb == NULL) {
        return;
    }
    self->running = pcb;
    load_context(self);
    printf("%u\t%u\t%s\t%s\n", self->cpu->clock, self->running->pid, "Ready", "Running");
    show_state(self);
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
    show_state(self);

    save_context(self);
    _heap_append(self->wait_queue, self->running);
    self->running = NULL;
    interrupt(self,ST_SCHEDULER);
}

// System call to End. Exit code is in cpu->mdr
void syscall_exit_request_callback(state_machine_t* self) {

    assert(self != NULL);
    //save_context(self);
    _heap_append(self->term_queue, self->running);
    self->running = NULL;
    interrupt(self,ST_SCHEDULER);
}

void show_state(state_machine_t* machine) {
    system("clear");
    printf("Clock: %d\n", machine->cpu->clock);
    _cpu_print(machine->cpu);
        
    // Print column headers
    printf("New Queue\tRead Queue\tRunning\t\tWait Queue\tTerm Queue\tReport Queue\n");

    heap_iterator_t* new_iter = _heap_iterator_create(machine->new_queue);
    heap_iterator_t* ready_iter = _heap_iterator_create(machine->ready_queue);
    heap_iterator_t* wait_iter = _heap_iterator_create(machine->wait_queue);
    heap_iterator_t* term_iter = _heap_iterator_create(machine->term_queue);
    heap_iterator_t* report_iter = _heap_iterator_create(machine->report_queue);

    int line = 0;
    while(_heap_iterator_has_next(new_iter) ||
        _heap_iterator_has_next(ready_iter) ||
        _heap_iterator_has_next(wait_iter) ||
        _heap_iterator_has_next(term_iter) ||
        _heap_iterator_has_next(report_iter)
    ) {
        line++;

        if (_heap_iterator_has_next(new_iter)) {
            job_t* job = _heap_iterator_next(new_iter);
            if (machine->cpu->clock >= job->arrival_time) {
                printf("1%d\t\t", job->pid);
            } else {
                printf("1\t\t");
            }
        } else {
            printf("1\t\t");
        }

        if (_heap_iterator_has_next(ready_iter)) {
            pcb_t* process = _heap_iterator_next(ready_iter);
            printf("2%d\t\t", process->pid);
        } else {
            printf("2\t\t");
        }

        if (machine->running != NULL && line == 1) {
            printf("3%d\t\t", machine->running->pid);
        } else {
            printf("3\t\t");
        }

        if (_heap_iterator_has_next(wait_iter)) {
            pcb_t* process = _heap_iterator_next(wait_iter);
            printf("4%d\t\t", process->pid);
        } else {
            printf("4\t\t");
        }

        if (_heap_iterator_has_next(term_iter)) {
            pcb_t* process = _heap_iterator_next(term_iter);
            printf("5%d\t\t", process->pid);
        } else {
            printf("5\t\t");
        }

        if (_heap_iterator_has_next(report_iter)) {
            pcb_t* process = _heap_iterator_next(report_iter);
            printf("6%d\t\t", process->pid);
        } else {
            printf("6\t\t");
        }

        printf("\n");
    }
    
    getchar();
    //usleep(1000000);
}

int main(int argc, char *argv[]) {

    char* file = NULL;
    char* schedule_type = NULL;
    if (argc != 3) {
        printf("Please run main <file> <schedule type>\n");
	    printf("<file> would be test_case_1.csv\n");
	    printf("<schedule type> would be FCFS, RR or Multi\n");
        file = "test_case_3.csv";
    } else {
        file = argv[1];
        schedule_type = argv[2];
    }

    // Create the CPU and state machine
    state_machine_t* machine = _state_machine_create(); 

    _state_machine_register_isr(machine, IRQ_TERMINATED, terminate_callback);
    _state_machine_register_isr(machine, IRQ_IO_COMPLETE, io_complete_callback);
    _state_machine_register_isr(machine, IRQ_LT_SCHEDULER_TIMEOUT, lt_scheduler_callback);
    _state_machine_register_isr(machine, ST_SCHEDULER, st_scheduler_callback);
    _state_machine_register_isr(machine, IRQ_CLOCK_PULSE, clock_pulse_callback);
    _state_machine_register_isr(machine, ADMITTED, admitted_callback);
    _state_machine_register_isr(machine, DISPATCH, dispatch_callback);
    _state_machine_register_isr(machine, PREEMPT, preempt_callback);
    _state_machine_register_isr(machine, SYSCALL_IO_REQUEST, syscall_io_request_callback);
    _state_machine_register_isr(machine, SYSCALL_EXIT_REQUEST, syscall_exit_request_callback);

    _job_load_from_file(file, machine->new_queue);
    int job_count = machine->new_queue->count;

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
            pcb_t* pcb = _heap_peak(machine->wait_queue);
            if (pcb != NULL) {
                pcb->remaining_io_cycles--;
                if (pcb->remaining_io_cycles == 0) {
                    interrupt(machine, IRQ_IO_COMPLETE);
                }
            }
        }
        // Generate a clock pulse
        interrupt(machine,IRQ_CLOCK_PULSE);

        if (job_count == machine->report_queue->count) {
            break;
        }
    }

    heap_iterator_t* iter = _heap_iterator_create(machine->report_queue);
    while (_heap_iterator_has_next(iter)) {
        pcb_t* process = _heap_iterator_next(iter);
        _pcb_print(process);
    }

    _state_machine_delete(machine);

    return 0;
 }
