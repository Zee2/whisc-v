// core.h

#ifndef CORE_H
#define CORE_H


#include <stdint.h>
#include "simulator.h"

#define REGFILE_SIZE 32

#define DO_BOUNDS_CHECK 1
#define NO_BOUNDS_CHECK 0

typedef struct core_state_t
{
    uint32_t pc_reg; // Program counter, points to next instruction
    uint32_t regfile[REGFILE_SIZE]; // Main regfile
} core_state_t;


int execute_rv32i(memory_t* memory, core_state_t* prev, core_state_t* next);

uint32_t fetch_width(memory_t* memory, uint32_t byte_addr, uint8_t width, uint8_t check);


#endif