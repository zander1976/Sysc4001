/**
 * Ben Woodhead 
 * 101057746
 * 
 * Emad Alomari
 * 101162157
 * 
 * Fall 2023 Sysc 4001
*/
#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

typedef enum {
    IRQ_TERMINATED, // OS has released the resources
    IRQ_IO_COMPLETE, // IOP has completed a task
    IRQ_LT_SCHEDULER_TIMEOUT, // (JOB Scheduler) timeout for the job or admitted scheduler 
    IRQ_CLOCK_PULSE, // Clock pulse
    
    ADMITTED, // Called from LT Schedulaer when a job is being admitted (move new -> ready)
    ST_SCHEDULER, // (CPU Scheduler) Selects the next process to run (orders the ready queue or has a variable to hold the next) 
    DISPATCH, // Kernel Mode Context switch and move the process into running (Ready to Running) 
    PREEMPT, // Kick process out of CPU

    SYSCALL_IO_REQUEST, // System call from User space to do an IO request. Wait time is in cpu->mdr)
    SYSCALL_EXIT_REQUEST, // System call to End. Exit code is in cpu->mdr
} state_codes_t;

typedef struct state_machine state_machine_t;
struct state_machine {
    surface_t* surface;
    cpu_t *cpu;
    heap_t *new_queue; 
    heap_t *memory_wait_queue;
    heap_t *ready_queue;
    heap_t *wait_queue;
    heap_t *term_queue;
    heap_t *report_queue;
    pcb_t *running;
    char* scheduler;
    void (*isr[SYSCALL_EXIT_REQUEST + 1])(state_machine_t* machine); 
};

state_machine_t* _state_machine_create(int col, int row, char* scheduler, int preempt, int* memory_blocks);
void _state_machine_delete(state_machine_t *self);
void _state_machine_register_isr(state_machine_t *self, state_codes_t state, void (*handler)(state_machine_t* hander));
void interrupt(state_machine_t *machine, state_codes_t state);
void save_context(state_machine_t* machine);
void load_context(state_machine_t* machine);
void promote_process(state_machine_t* machine);
void demote_processb(state_machine_t* machine);

#endif //__SCHEDULAR_H__

#ifdef __STATE_MACHINE_IMPLEMENTATION__

state_machine_t* _state_machine_create(int col, int row, char* scheduler, int preempt, int* memory_blocks) {

    state_machine_t* machine = malloc(sizeof(state_machine_t));
    assert(machine != NULL);

    machine->surface = _render_create_surface(col, row);
    assert(machine->surface != NULL);

    machine->scheduler = scheduler;

    machine->cpu = _cpu_create(preempt);
    machine->running = NULL;
    //machine->main_memory = _main_memory_create(16);

    // Create all the queues
    machine->new_queue    = _heap_create(4, _job_arrival_time_compare_func);
    machine->memory_wait_queue = _heap_create(4, _job_arrival_time_compare_func);
    machine->ready_queue  = _heap_create(4, _pcb_priority_compare_func);
    machine->wait_queue   = _heap_create(4, _pcb_fcfs_compare_func);
    machine->term_queue   = _heap_create(4, _pcb_fcfs_compare_func);
    machine->report_queue = _heap_create(4, _pcb_fcfs_compare_func);

    return machine;
}

void _state_machine_delete(state_machine_t *self) {

    _heap_delete(self->new_queue);
    _heap_delete(self->memory_wait_queue);
    _heap_delete(self->ready_queue);
    _heap_delete(self->wait_queue);
    _heap_delete(self->term_queue);
    _heap_delete(self->report_queue);

    _cpu_delete(self->cpu);

    _render_delete_surface(self->surface);

    free(self);
}

void _state_machine_register_isr(state_machine_t *self, state_codes_t state, void (*handler)(state_machine_t* hander)) {
    assert(self != NULL);
    assert(state <= SYSCALL_EXIT_REQUEST+1);
    assert(handler != NULL);
    self->isr[state] = handler;
}

void interrupt(state_machine_t *machine, state_codes_t state) {
    assert(machine != NULL);
    assert(state <= SYSCALL_EXIT_REQUEST+1);

    if (machine->isr[state] == NULL) {
        return;
    }
    machine->isr[state](machine);
}

// Save CPU state to process
void save_context(state_machine_t* machine) {
    assert(machine != NULL);
    machine->running->program_counter = machine->cpu->program_counter;
} 

// Load 
void load_context(state_machine_t* machine) {
    assert(machine != NULL);
    assert(machine->running != NULL);
    machine->cpu->program_counter = machine->running->program_counter;
    machine->cpu->process_id = machine->running->pid;
} 

void promote_process(state_machine_t* machine) {
    assert(machine != NULL);
}

void demote_processb(state_machine_t* machine) {
    assert(machine != NULL);
}

#endif // __SCHEDULAR_IMPLEMENTATION__
