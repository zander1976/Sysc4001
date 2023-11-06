/**
 * Ben Woodhead 
 * 101057746
 * 
 * Emad Alomari
 * 101162157
 * 
 * Fall 2023 Sysc 4001
*/
#ifndef __FETCH_EXEC_H__
#define __FETCH_EXEC_H__

void execute_opcode(cpu_t* cpu, main_memory_t* main_memory, state_machine_t* machine);

#endif // __FETCH_EXEC_H__

#ifdef __FETCH_EXEC_IMPLEMENTATION__
void execute_opcode(cpu_t* cpu, main_memory_t* main_memory, state_machine_t* machine) {
    unsigned int offset = machine->running->text_start_address;
    unsigned int ops = _main_memory_get(main_memory, offset, cpu->program_counter);
    unsigned int opcode = ops & 0xFF00;
    unsigned int data = ops & 0x00FF;
    switch(opcode) {
        case JOB:
            //printf("%x\tBegin Job %d\n", cpu->program_counter, _main_memory_get(main_memory, offset, data));
            break;
        case RUN: 
            // Set the process to user mode
            //printf("%x \tRun\n", cpu->program_counter);
            break;
        case NOP:
            //printf("%x \t\tNOP\n", cpu->program_counter);
            break;
        case READ:
            cpu->mdr = _main_memory_get(main_memory, offset, data);
            //printf("%x \t\tRead\tWait:\t%d\n", cpu->program_counter, cpu->mdr);
            system_call(cpu, main_memory, machine, REQUEST_READ);
            break;
        case END:
            //printf("%x \tEnd Job %d\n", cpu->program_counter, _main_memory_get(main_memory, offset, data));
            system_call(cpu, main_memory, machine, REQUEST_TERMINATE);
            break;
    }
}

#endif // __FETCH_EXEC_IMPLEMENTATION__