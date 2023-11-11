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

void _job_load_from_file(const char *filename,  heap_t* queue);
void _job_print(job_t* job);

// If left is smaller then right
bool _job_arrival_time_compare_func(const void *left , const void *right);

#endif //__JOB_SPOOL_H__

#ifdef __JOB_SPOOL_IMPLEMENTATION__

// simultaneous peripheral operations on-line
// temporarily held to be used and executed by a device, program or the system
void _job_load_from_file(const char *filename,  heap_t* queue) {
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

        _heap_append(queue, job);
    }

    fclose(f);
}

bool _job_arrival_time_compare_func(const void *left , const void *right) {
    job_t* job_left = (job_t*)left;
    job_t* job_right = (job_t*)right;
    
    if (job_left->arrival_time < job_right->arrival_time) {
        return true;
    }
    return false;
}

#endif //__SPOOL_IMPLEMENTATION__