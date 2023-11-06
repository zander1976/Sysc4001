/**
 * Ben Woodhead 
 * 101057746
 * 
 * Emad Alomari
 * 101162157
 * 
 * Fall 2023 Sysc 4001
*/
#ifndef __OPCODES_H__
#define __OPCODES_H__

// OPCode table
typedef enum {
  // OS job management opcodes
  JOB = 0x1000, // Tells the OS that a job is being defined
  RUN = 0x1100, // Transfers control to user program
  END = 0x1200, // Tells the OS that a job is ending
  
  READ = 0x1300, // Read From IO

  // User level iocodes
  NOP = 0x0100,
} opcode_t;

#endif // __OPCODES_H__
