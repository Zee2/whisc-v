// core.c

#include "core.h"
#include "decode.h"
#include "opcodes.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#define GET_MATH_BIT(x) ((x >> 30) & 0x1)

// Forward decls of local functions

// Mutates regfile according to reg-reg instruction parameters
int execute_r_type(uint32_t instruction_bits, uint8_t rd, uint8_t rs1, uint8_t rs2, uint8_t funct3, uint32_t* regfile);

int execute_rv32i(uint32_t* program_data, core_state_t* prev, core_state_t* next){
    if(prev == NULL || next == NULL || program_data == NULL){
        return -1;
    }
    if(prev != next){
        memcpy(next, prev, sizeof(core_state_t));
    }

    // Fetch the instruction
    uint32_t instruction_bits = *(program_data + prev->pc_reg);

    next->pc_reg += 1;

    printf("Full instruction: %08x:  ", instruction_bits);

    // Decode the instruction
    instruction_rv32i_t decoded_ins;
    decode_rv32i(instruction_bits, &decoded_ins);

    printf("Opcode: 0x%02x - ", decoded_ins.opcode);
    
    // Execute the instruction
    switch (decoded_ins.ins_type)
    {
    case r_type: 
        printf("REG: rd=%d, rs1=%d, rs2=%d, funct3=%d\n", decoded_ins.r_data.rd,
                                decoded_ins.r_data.rs1,
                                decoded_ins.r_data.rs2,
                                decoded_ins.r_data.funct3);
        return execute_r_type(instruction_bits,
                                decoded_ins.r_data.rd,
                                decoded_ins.r_data.rs1,
                                decoded_ins.r_data.rs2,
                                decoded_ins.r_data.funct3,
                                next->regfile);
    
    default:
        printf(" NOP \n");
        break;
    }
    return 0;
}

int execute_r_type(uint32_t instruction_bits, uint8_t rd, uint8_t rs1, uint8_t rs2, uint8_t funct3, uint32_t* regfile){
    switch(funct3)
    {
        // ADD/SUB
        case 0x0: 
        {
            int sign_bit = GET_MATH_BIT(instruction_bits) == 0 ? 1 : -1;
            regfile[rd] =
                (int32_t)regfile[rs1] + sign_bit * (int32_t)regfile[rs2];
            break;
        }
        // SLL (Shift Left Logical)
        case 0x1:
            regfile[rd] =
                regfile[rs1] << regfile[rs2];
            break;
        // SLT (Set if Less Than)
        case 0x2:
            regfile[rd] = 
                (int32_t)regfile[rs1] < (int32_t)regfile[rs2];
            break;
        // SLTU (Set if Less Than, Unsigned)
        case 0x3:
            regfile[rd] = 
                (uint32_t)regfile[rs1] < (uint32_t)regfile[rs2];
            break;
        // XOR
        case 0x4:
            regfile[rd] = 
                regfile[rs1] ^ regfile[rs2];
            break;
        // SRL, SRA (Shift right, Logical or Arithmetic)
        case 0x5:
            // If bit30 is zero, it is a logical (unsigned) shift
            if(GET_MATH_BIT(instruction_bits) == 0){
                regfile[rd] =
                    (uint32_t)regfile[rs1] >> (uint32_t)regfile[rs2];
            } else { // Else, arithmetic/signed shift
                regfile[rd] =
                    (int32_t)regfile[rs1] >> (int32_t)regfile[rs2];
            }
            break;
        // OR
        case 0x6:
            regfile[rd] = 
                regfile[rs1] | regfile[rs2];
            break;
        // AND
        case 0x7:
            regfile[rd] = 
                regfile[rs1] & regfile[rs2];
            break;
        default:
            return -1;
    }
    return 0;
}