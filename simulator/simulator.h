// simulator.h

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdint.h>

#define MEM_SIZE 4096

typedef struct memory_t {
    uint8_t data[MEM_SIZE];
    uint32_t mem_lower_bound;
    uint32_t mem_upper_bound;

} memory_t;

#endif