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
    cpu_t *cpu;
    queue_t *ready_queue;
    queue_t *wait_queue;
    queue_t *term_queue;
    queue_t *job_spool; // New queue
    queue_t *report_queue; 
    pcb_t *running;
    int preempt;
    void (*isr[SYSCALL_EXIT_REQUEST + 1])(state_machine_t* machine); 
};

state_machine_t* _state_machine_create(char* schedule_type, int count, ...);
void _state_machine_delete(state_machine_t *self);
void _state_machine_register_isr(state_machine_t *self, state_codes_t state, void (*handler)(state_machine_t* hander));
void interrupt(state_machine_t *machine, state_codes_t state);
void save_context(state_machine_t* machine);
void load_context(state_machine_t* machine);

#endif //__SCHEDULAR_H__

#ifdef __STATE_MACHINE_IMPLEMENTATION__

state_machine_t* _state_machine_create(char* schedule_type, int count, ...) {
    printf("%s\n", schedule_type);
    va_list valist;
    va_start(valist, count);
    for (int i = 0; i < count; i++) {
        printf("%d\n", va_arg(valist, int));
    }
    va_end(valist);
    exit(0);

    state_machine_t* machine = malloc(sizeof(state_machine_t));
    assert(machine != NULL);

    machine->cpu = _cpu_create();
    //machine->main_memory = _main_memory_create(16);
    machine->ready_queue = _queue_create();
    machine->wait_queue = _queue_create();
    machine->term_queue = _queue_create();
    machine->job_spool = _queue_create();
    machine->report_queue = _queue_create();
    
    machine->preempt = 0;

    machine->running = NULL;

    return machine;
}

void _state_machine_delete(state_machine_t *self) {

    _queue_delete(self->ready_queue);
    _queue_delete(self->wait_queue);
    _queue_delete(self->term_queue);
    _queue_delete(self->job_spool);
    _queue_delete(self->report_queue);

    _cpu_delete(self->cpu);
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
    machine->running->flags = machine->cpu->flags;
} 

// Load 
void load_context(state_machine_t* machine) {
    assert(machine != NULL);
    assert(machine->running != NULL);
    machine->cpu->program_counter = machine->running->program_counter;
    machine->cpu->flags = machine->running->flags;
    machine->cpu->process_id = machine->running->pid;
} 

#endif // __SCHEDULAR_IMPLEMENTATION__
