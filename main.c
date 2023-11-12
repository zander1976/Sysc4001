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
#include <string.h>

#define __HEAP_IMPLEMENTATION__
#include "util_heap.h"

#define __ITER_IMPLEMENTATION__
#include "util_iter.h"

#define __JOB_SPOOL_IMPLEMENTATION__
#include "job_spool.h"

#define __PCB_IMPLEMENTATION__
#include "pcb.h"

#define __HARDWARE_IMPLEMENTATION__
#include "hardware.h"

#define __RENDER_IMPLEMENTATION__
#include "render.h"

#define __STATE_MACHINE_IMPLEMENTATION__
#include "state_machine.h"

void show_state(state_machine_t* machine);

// OS has released the resources
void terminate_callback(state_machine_t* self) {
    assert(self != NULL);
    pcb_t* process = NULL; 
    while( (process = _heap_pop(self->term_queue)) != NULL) {
        // Announce it's deletion
        _heap_append(self->report_queue, process);
        //printf("%u\t%u\t%s\t%s\n", self->cpu->clock, process->pid, "Running", "Terminated");
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
    //printf("%u\t%u\t%s\t%s\n", self->cpu->clock, pcb->pid, "Waiting", "Ready");
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
    show_state(self);

    // Release any terminated processes
    if (self->term_queue->count != 0) {
        // Call terminated
        interrupt(self, IRQ_TERMINATED);
    }

    // We can select how often we want to run this
    if ((self->cpu->clock % 1) == 0) {
        interrupt(self, IRQ_LT_SCHEDULER_TIMEOUT);
    }
    
    // Check the wait queue
    // Increment the wait counter
    heap_iterator_t* iter = _heap_iterator_create(self->wait_queue);
    while (_heap_iterator_has_next(iter)) {
        pcb_t* process = _heap_iterator_next(iter);
        process->remaining_io_cycles--;
        if (process->remaining_io_cycles == 0) {
            interrupt(self, IRQ_IO_COMPLETE);
        }
    }
    _heap_iterator_delete(iter);

    self->cpu->clock++;
    self->cpu->program_counter++;

    // Not sure if this should be before or after
    if (self->cpu->preempt != 0) {
        if ((self->cpu->program_counter % self->cpu->preempt) == 0) {
            interrupt(self, PREEMPT);
        }
    }    

    // Increment the wait counter
    iter = _heap_iterator_create(self->ready_queue);
    while (_heap_iterator_has_next(iter)) {
        pcb_t* process = _heap_iterator_next(iter);
        process->total_wait_time += 1;
    }
    _heap_iterator_delete(iter);

    if (self->running != NULL) {
        if (self->cpu->program_counter == self->running->total_cpu_time) {
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

    job_t* job = _heap_pop(self->new_queue);
    pcb_t* process = malloc(sizeof(pcb_t));
    if (process == NULL) {
        return;
    }

    _pbc_admit_job(process, job);

    //printf("%u\t%u\t%s\t%s\n", self->cpu->clock, job->pid, "New", "Ready");

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
    //printf("%u\t%u\t%s\t%s\n", self->cpu->clock, self->running->pid, "Ready", "Running");
    show_state(self);
}

// Kick process out of CPU
void preempt_callback(state_machine_t* self) {
    assert(self != NULL);
    if (self->running == NULL) {
        return;
    }

    //printf("%u\t%u\t%s\t%s\n", self->cpu->clock, pcb->pid, "Running", "Ready");
    _heap_append(self->ready_queue, self->running);
    self->running = NULL;
    interrupt(self, ST_SCHEDULER);
}

// System call from User space to do an IO request. Wait time is in cpu->mdr)
void syscall_io_request_callback(state_machine_t* self) {
    assert(self != NULL);

    self->running->remaining_io_cycles = self->running->io_duration;
    //printf("%u\t%u\t%s\t%s\n", self->cpu->clock, self->running->pid, "Running", "Waiting");

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

    _render_clear_surface(machine->surface);

    _render_write_string(machine->surface, 0, 0, "Time: ", COLOR_WHITE);
    _render_write_int(machine->surface, 6, 0, machine->cpu->clock, COLOR_CYAN);

    _render_write_string(machine->surface, 0, 1, "Scheduler: ", COLOR_WHITE);
    _render_write_string(machine->surface, 11, 1, machine->scheduler, COLOR_CYAN);

    _render_write_string(machine->surface, 0, 2, "Preempt: ", COLOR_WHITE);
    if (machine->cpu->preempt == 0) {
        _render_write_string(machine->surface, 9, 2, "No", COLOR_CYAN);
    } else {
        _render_write_string(machine->surface, 9, 2, "Yes", COLOR_CYAN);
    }


    int x = 0;
    int y = 5;
    _render_write_string(machine->surface, x, y, "New Queue:", COLOR_WHITE);
    _render_write_string(machine->surface, x, y+1, "PID   Arrival Priority", COLOR_WHITE);
    heap_iterator_t* new_iter = _heap_iterator_create(machine->new_queue);
    for(int i=1; i <= 10; i++) {
        if (_heap_iterator_has_next(new_iter) == false) {
            break;
        } 
        job_t* job = _heap_iterator_next(new_iter);
        char output[22];
        sprintf(output, "%d     %d       %d", job->pid, job->arrival_time, job->priority);
        _render_write_string(machine->surface, x, y+1+i, output, COLOR_CYAN);
    }
    _heap_iterator_delete(new_iter);

    x = 0;
    y = 17;
    _render_write_string(machine->surface, x, y, "Wait Queue:", COLOR_WHITE);
    _render_write_string(machine->surface, x, y+1, "PID  IO Duration IO Remaining", COLOR_WHITE);
    heap_iterator_t* wait_iter = _heap_iterator_create(machine->wait_queue);
    for(int i=1; i <= 10; i++) {
        if (_heap_iterator_has_next(wait_iter) == false) {
            break;
        } 
        pcb_t* process = _heap_iterator_next(wait_iter);
        char output[35];
        sprintf(output, "%d       %d            %d", process->pid, process->io_duration, process->remaining_io_cycles);
        _render_write_string(machine->surface, x, y+1+i, output, COLOR_CYAN);
    }
    _heap_iterator_delete(wait_iter);

    x = 0;
    y = 29;
    _render_write_string(machine->surface, x, y, "Term Queue:", COLOR_WHITE);
    _render_write_string(machine->surface, x, y+1, "PID  Turnaround Avg Wait Avg Response", COLOR_WHITE);
    heap_iterator_t* term_iter = _heap_iterator_create(machine->term_queue);
    for(int i=1; i <= 10; i++) {
        if (_heap_iterator_has_next(term_iter) == false) {
            break;
        } 
        pcb_t* process = _heap_iterator_next(term_iter);
        char output[35];
        sprintf(output, "%d     %d        %.2f       %.2f", process->pid, process->departed_time-process->arrival_time, (float)process->total_wait_time / process->wait_count, (float)process->total_response_time / process->response_count);
        _render_write_string(machine->surface, x, y+1+i, output, COLOR_CYAN);
    }
    _heap_iterator_delete(term_iter);

    x = 35;
    y = 5;
    _render_write_string(machine->surface, x, y, "Read Queue:", COLOR_WHITE);
    _render_write_string(machine->surface, x, y+1, "PID   CPU Left  Priority", COLOR_WHITE);
    heap_iterator_t* ready_iter = _heap_iterator_create(machine->ready_queue);
    for(int i=1; i <= 7; i++) {
        if (_heap_iterator_has_next(ready_iter) == false) {
            break;
        } 
        pcb_t* process = _heap_iterator_next(ready_iter);
        char output[22];
        sprintf(output, "%d       %d      %d", process->pid, process->total_cpu_time-process->program_counter, process->priority);
        _render_write_string(machine->surface, x, y+1+i, output, COLOR_CYAN);
    }
    _heap_iterator_delete(ready_iter);

    x = 70;
    y = 5;
    _render_write_string(machine->surface, x, y, "Running:", COLOR_WHITE);
    if (machine->running != NULL) {
        pcb_t* process = machine->running;
        char output[22];
        if (machine->cpu->preempt == 0) {
            _render_write_string(machine->surface, x, y+1, "PID    CPU Left  Priority", COLOR_WHITE);
            sprintf(output, "%d      %d        %d", process->pid, process->total_cpu_time - process->program_counter, process->priority);
        } else {
            _render_write_string(machine->surface, x, y+1, "PID    CPU Left  Preempt", COLOR_WHITE);
            sprintf(output, "%d      %d        %d", process->pid, process->program_counter, machine->cpu->preempt);
        }
        _render_write_string(machine->surface, x, y+2, output, COLOR_YELLOW);
    }

    // Display the results
    _render_display_frame(machine->surface);
    getchar();
    //usleep(1000000);
}

int main(int argc, char *argv[]) {

    char* file = NULL;
    char* scheduler = NULL;
    int preempt = 0;
    int memory_block_count;
    char* memory = NULL;
    int *memory_blocks = NULL;

    if (argc < 5) {
        printf("\n\nmain <filename> <scheduler> <preempt> <memory_block_count> <memory_layout>\n\n");
        printf("<filename> is the csv file.\n");
        printf("<scheduler> FCFS PRIORITY RR\n");
        printf("<preempt> Preempt time, 0 for none\n");
        printf("<memory_block_count> How many blocks of memory 0 for infinit\n");
        printf("<memory> 102,123,12,14\n\n");
        exit(0);
    } else {
        file = argv[1];
        scheduler = argv[2];
        preempt = atoi(argv[3]);
        memory_block_count = atoi(argv[4]);
        if (memory_block_count == 0) {
            printf("%s %s %d\n", file, scheduler, preempt);
        } else {
            memory_blocks = (int *)malloc(memory_block_count * sizeof(int));
            assert(memory_blocks);

            char *token = strtok(argv[5], ",");
            
            // Populate the array with integers
            int index = 0;
            while (token != NULL) {
                memory_blocks[index] = atoi(token);
                index++;
                token = strtok(NULL, ",");
            }

            printf("%s %s %d ", file, scheduler, preempt);
            for (int i = 0; i < memory_block_count; i++) {
                printf("%d ", memory_blocks[i]);
            }            
            printf("\n");
        }
    }

    // Create the CPU and state machine
    state_machine_t* machine = _state_machine_create(160, 48, scheduler, preempt, memory_blocks); 

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

        // Generate a clock pulse
        interrupt(machine,IRQ_CLOCK_PULSE);

        if (job_count == machine->report_queue->count) {
            break;
        }
    }

    heap_iterator_t* iter = _heap_iterator_create(machine->report_queue);
    while (_heap_iterator_has_next(iter)) {
        pcb_t* process = _heap_iterator_next(iter);
        //_pcb_print(process);
    }
    _heap_iterator_delete(iter);

    _state_machine_delete(machine);

    free(memory_blocks);

    return 0;
 }
