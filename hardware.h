/**
 * Ben Woodhead 
 * 101057746
 * 
 * Emad Alomari
 * 101162157
 * 
 * Fall 2023 Sysc 4001
*/
#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>

typedef struct {
    unsigned int process_id;
    unsigned int program_counter;
    unsigned int flags;
    unsigned int interrupt;
    unsigned int clock;
    unsigned int mdr;
} cpu_t;

typedef struct {
    size_t count;
    size_t length; 
    unsigned int *data;
} main_memory_t;

cpu_t* _cpu_create();
void _cpu_delete(cpu_t* self);
void _cpu_print(cpu_t* self);
main_memory_t* _main_memory_create(size_t start_size);
void _main_memory_delete(main_memory_t* self);
void _main_memory_dump(main_memory_t* self);
void _main_memory_resize(main_memory_t* self);
void _main_memory_append(main_memory_t* self, unsigned int *data, size_t count);
unsigned int _main_memory_get(main_memory_t* self, unsigned int offset, unsigned int position);
void _main_memory_remove(main_memory_t* self, size_t start, size_t count);

#endif // __HARDWARE_H__

#ifdef __HARDWARE_IMPLEMENTATION__

cpu_t* _cpu_create() {
    cpu_t* cpu = malloc(sizeof(cpu_t));
    assert(cpu != NULL);
    cpu->process_id = 0;
    cpu->program_counter = 0;
    cpu->flags = 0;
    cpu->interrupt = 0;
    cpu->clock = 0;
    cpu->mdr = 0;
    return cpu;
}

void _cpu_delete(cpu_t* self) {
    assert(self != NULL);
    free(self);
}

void _cpu_clear(cpu_t* self) {
    self->process_id = 0;
    self->program_counter = 0;
    self->flags = 0;
    self->interrupt = 0;
    self->mdr = 0;
}

void _cpu_print(cpu_t* self) {
    assert(self != NULL);
    printf("\n------------------\n");
    printf("Process Id: %d\n", self->process_id);
    printf("Program Counter: %d\n", self->program_counter);
    printf("Flags: %d\n", self->flags);
    printf("Interrupts: %d\n", self->interrupt);
    printf("Clock: %d\n", self->clock);
    printf("Data Register: %d\n", self->mdr);
    printf("------------------\n\n");
}

main_memory_t* _main_memory_create(size_t start_size) {
    assert(start_size > 0);

    main_memory_t* memory = malloc(sizeof(main_memory_t));
    assert(memory != NULL);

    memory->count = 0;
    memory->length = start_size;

    memory->data = malloc(memory->length * sizeof(unsigned int));
    assert(memory->data != NULL);

    for(size_t i = 0; i < memory->length; i++) {
        memory->data[i] = 0xFFFF;
    }

    return memory;
}

void _main_memory_delete(main_memory_t* self) {
    assert(self != NULL);
    free(self->data);
    free(self);
}

void _main_memory_dump(main_memory_t* self) {
    assert(self != NULL);
    printf("\n------------------\n");
    for(size_t i = 0; i < self->length; i++) {
        printf("%zu\t%x\n", i, self->data[i]);
    }
    printf("------------------\n\n");
}

void _main_memory_resize(main_memory_t* self) {
    assert(self != NULL);
    self->length *= 2;
    self->data = realloc(self->data, self->length * sizeof(unsigned int));
    assert(self->data != NULL);

    for(size_t i = self->count; i < self->length; i++) {
        self->data[i] = 0xFFFF;
    }

}

void _main_memory_append(main_memory_t* self, unsigned int *data, size_t count) {
    assert(self != NULL);
    assert(data != NULL);
    assert(count != 0);
    
    if ((self->count+count) > self->length) {
        _main_memory_resize(self);
    }

    memcpy(self->data + self->count, data, count * sizeof(unsigned int));
    self->count += count;
}
unsigned int _main_memory_get(main_memory_t* self, unsigned int offset, unsigned int position) {
    assert(self != NULL);
    //printf("Memory Address: %u\n", offset + position);
    return self->data[offset + position];
}


void _main_memory_remove(main_memory_t* self, size_t start, size_t count) {
    assert(self != NULL);
    assert(start != 0);
    assert(count != 0);
    assert((start+count) < self->length);
    
    for(size_t i = 0; i < count; i++) {
        self->data[start+i] = 0xFFFF;
    }
    self->count -= count;
}

#endif // __HARDWARE_IMPLEMENTATION__