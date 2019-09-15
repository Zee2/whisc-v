// core.h

#ifndef CORE_H
#define CORE_H

#define REGFILE_SIZE 32

#include <stdint.h>

typedef struct core_state_t
{
    uint32_t pc_reg; // Program counter, points to next instruction
    uint32_t regfile[REGFILE_SIZE]; // Main regfile
} core_state_t;


int execute_rv32i(uint32_t* program_data, core_state_t* prev, core_state_t* next);



#endif