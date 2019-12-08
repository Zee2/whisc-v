// detangle.h
// Utility functions for detangling
// crazy RISC-V immediate bit vectors

#include "detangle.h"
#include "opcodes.h"
#include <stdint.h>

uint32_t detangle_rv32i(uint32_t word, ins_types_rv32i_t type) {
    #ifndef NO_VALID_CHECKS
    if(type == r_type){
        return 0;
    }
    #endif

    switch (type)
    {
        case i_type:
            return (word >> 20) & 0xFFF;
        case s_type:
            return ((word >> 7) & 0x1F) | ((word >> 25) & 0x7F);
        case b_type:
        {
            uint8_t bit_12 = (word >> 31) & 0x1;
            uint8_t bit_11 = (word >> 7) & 0x1;
            uint8_t bit_10_5 = (word >> 25) & 0x3F;
            uint8_t bit_4_1 = (word >> 8) & 0x0F;
            return (bit_4_1 << 1) | (bit_10_5 << 5) | (bit_11 << 11) | (bit_12 << 12);
        }
        case u_type:
            return word & 0xFFFFF000;
        case j_type:
        {   
            uint32_t bit_20 = (word >> 31) & 0x1;
            uint32_t bit_10_1 = (word >> 21) & 0x3FF;
            uint32_t bit_11 = (word >> 20) & 0x1;
            uint32_t bit_19_12 = (word >> 12) & 0xFF;
            return (bit_10_1 << 1) | (bit_11 << 11) | (bit_19_12 << 12) | (bit_20 << 20);
        }
        default:
            return 0;
    }
}