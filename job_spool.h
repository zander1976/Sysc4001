/**
 * Ben Woodhead 
 * 101057746
 * 
 * Emad Alomari
 * 101162157
 * 
 * Fall 2023 Sysc 4001
*/
#ifndef __JOB_SPOOL_H__
#define __JOB_SPOOL_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Define programs stored in spool
typedef struct {
    unsigned int pid;
    unsigned int arrival_time;
    unsigned int total_cpu;
    unsigned int io_frequency;
    unsigned int io_duration;
    unsigned int priority;
} job_t;

void _job_load_from_file(const char *filename,  queue_t* queue);
void _job_print(job_t* job);

#endif //__JOB_SPOOL_H__

#ifdef __JOB_SPOOL_IMPLEMENTATION__

// simultaneous peripheral operations on-line
// temporarily held to be used and executed by a device, program or the system
void _job_load_from_file(const char *filename,  queue_t* queue) {
    assert(filename != NULL);
    assert(queue != NULL);

    FILE *f = fopen(filename, "r");
    assert(f != NULL);

    char buffer[256];
    
    // Skip the first line
    fgets(buffer, sizeof(buffer), f);

    // Create a list of programs
    while(fgets(buffer, sizeof(buffer), f) != NULL) {
        job_t *job = malloc(sizeof(job_t));

        char *token = strtok(buffer, ",");
        job->pid = atoi(token);

        token = strtok(NULL, ",");
        job->arrival_time = atoi(token);

        token = strtok(NULL, ",");
        job->total_cpu = atoi(token);

        token = strtok(NULL, ",");
        job->io_frequency = atoi(token);
 
        token = strtok(NULL, ",");
        job->io_duration = atoi(token);

        token = strtok(NULL, ",");
        job->priority = atoi(token);

        _queue_push(queue, job);
    }

    fclose(f);
}

void _job_print(job_t* job) {
    printf("----------------------------\n");
    printf("PID: %d\n", job->pid);
    printf("Arrival Time: %d\n", job->arrival_time);
    printf("Total CPU: %d\n", job->total_cpu);
    printf("IO Frequency: %d\n", job->io_frequency);
    printf("IO Duration: %d\n", job->io_duration);
    printf("----------------------------\n");
}


#endif //__SPOOL_IMPLEMENTATION__