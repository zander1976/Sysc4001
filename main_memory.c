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


int main() {
    int list[] = {100,100,50};
    main_memory_t* main_memory = _main_memory_create(sizeof(list)/sizeof(int), list);

    for(int i = 0; i < 5; i++) {
        job_t* job = malloc(sizeof(job_t));
        job->pid = 1000 + i;
        job->memory_size = 100;
        if (i == 3) {
            job->memory_size = 50;
        }
        if (_main_memory_is_fit_possible(main_memory, job) == false) {
            printf("Job %d won't fit: \n", job->pid);
            continue;
        }
        pcb_t* process = malloc(sizeof(pcb_t));
        _pbc_admit_job(process, job);

        if (_main_memory_check_availability(main_memory, process) == false) {
            printf("No availibity for: %d\n", i);
            continue;
        }

        if (_main_memory_append(main_memory, process) == false) {
            printf("Failed to fit: %d\n", i);
            continue;
        }
        printf("Added - PIDS: %d Location: %d\n", process->pid, process->memory_location);
    }

    heap_iterator_t* iter = _heap_iterator_create(main_memory->memory_blocks);
    while (_heap_iterator_has_next(iter)) {
        memory_frag_t* frag = _heap_iterator_next(iter);
        if (frag->data != NULL) {
            pcb_t* process = (pcb_t*)frag->data;
            printf("PIDS: %d Location: %d\n", process->pid, process->memory_location);
        }
    }
    _heap_iterator_delete(iter);

    pcb_t* process1 = malloc(sizeof(pcb_t));
    process1->pid = 1003;
    _main_memory_remove(main_memory, process1);

    iter = _heap_iterator_create(main_memory->memory_blocks);
    while (_heap_iterator_has_next(iter)) {
        memory_frag_t* frag = _heap_iterator_next(iter);
        if (frag->data != NULL) {
            pcb_t* process = (pcb_t*)frag->data;
            printf("PIDS: %d Location: %d\n", process->pid, process->memory_location);
        }
    }
    _heap_iterator_delete(iter);

    //bool _main_memory_remove(main_memory_t* self, pcb_t* process);
    return 0;
}