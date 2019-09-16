// core.c

#include "core.h"
#include "decode.h"
#include "opcodes.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>


// Forward decls of local functions

// Mutates regfile according to reg-reg instruction parameters
int execute_r_type(uint32_t instruction_bits, r_type_rv32i_t data, uint32_t* regfile);

int execute_i_type(uint32_t instruction_bits, opcode_rv32i_t opcode, i_type_rv32i_t data, uint32_t* regfile);

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
                                decoded_ins.r_data,
                                next->regfile);
    case i_type:
        printf("IMM: rd=%d, rs1=%d, imm12=%d, funct3=%d\n", decoded_ins.i_data.rd,
                                decoded_ins.i_data.rs1,
                                decoded_ins.i_data.imm12,
                                decoded_ins.i_data.funct3);
        return execute_i_type(instruction_bits,
                                decoded_ins.opcode,
                                decoded_ins.i_data,
                                next->regfile);
    default:
        printf(" NOP \n");
        break;
    }
    return 0;
}

int execute_r_type(uint32_t instruction_bits, r_type_rv32i_t data, uint32_t* regfile){
    switch(data.funct3)
    {
        // ADD/SUB
        case 0x0: 
        {
            int sign_bit = GET_MATH_BIT(instruction_bits) == 0 ? 1 : -1;
            regfile[data.rd] =
                (int32_t)regfile[data.rs1] + sign_bit * regfile[data.rs2];
            break;
        }
        // SLL (Shift Left Logical)
        case 0x1:
            regfile[data.rd] =
                regfile[data.rs1] << regfile[data.rs2];
            break;
        // SLT (Set if Less Than)
        case 0x2:
            regfile[data.rd] = 
                (int32_t)regfile[data.rs1] < (int32_t)regfile[data.rs2];
            break;
        // SLTU (Set if Less Than, Unsigned)
        case 0x3:
            regfile[data.rd] = 
                (uint32_t)regfile[data.rs1] < (uint32_t)regfile[data.rs2];
            break;
        // XOR
        case 0x4:
            regfile[data.rd] = 
                regfile[data.rs1] ^ regfile[data.rs2];
            break;
        // SRL, SRA (Shift right, Logical or Arithmetic)
        case 0x5:
            // If bit30 is zero, it is a logical (unsigned) shift
            if(GET_MATH_BIT(instruction_bits) == 0){
                regfile[data.rd] =
                    (uint32_t)regfile[data.rs1] >> (uint32_t)regfile[data.rs2];
            } else { // Else, arithmetic/signed shift
                regfile[data.rd] =
                    (int32_t)regfile[data.rs1] >> (int32_t)regfile[data.rs2];
            }
            break;
        // OR
        case 0x6:
            regfile[data.rd] = 
                regfile[data.rs1] | regfile[data.rs2];
            break;
        // AND
        case 0x7:
            regfile[data.rd] = 
                regfile[data.rs1] & regfile[data.rs2];
            break;
        default:
            return -1;
    }
    return 0;
}

int execute_i_type(uint32_t instruction_bits, opcode_rv32i_t opcode, i_type_rv32i_t data, uint32_t* regfile){
    if(opcode == OP_IMM) {
        switch (data.funct3)
        {
        // Sign-extended addition, immediate
        case IMM_ADDI:
            regfile[data.rd] = (int32_t)regfile[data.rs1] + SIGN_EXTEND(data.imm12, 12);
            break;
        // Set if less than, immediate
        case IMM_SLTI:
            regfile[data.rd] = (int32_t)regfile[data.rs1] < SIGN_EXTEND(data.imm12, 12);
            break;
        // Set if less than, immediate, unsigned
        case IMM_SLTIU:
            regfile[data.rd] = (uint32_t)regfile[data.rs1] < (uint32_t)data.imm12;
            break;
        // Bitwise XOR, immediate, sign-extended
        case IMM_XORI:
            regfile[data.rd] = (int32_t)regfile[data.rs1] ^ SIGN_EXTEND(data.imm12, 12);
            break;
        // Bitwise OR, immediate, sign-extended
        case IMM_ORI:
            regfile[data.rd] = (int32_t)regfile[data.rs1] | SIGN_EXTEND(data.imm12, 12);
            break;
        // Bitwise AND, immediate, sign-extended
        case IMM_ANDI:
            regfile[data.rd] = (int32_t)regfile[data.rs1] & SIGN_EXTEND(data.imm12, 12);
        default:
            printf("Illegal immediate funct3 code: %03x", data.funct3);
            break;
        }
    } else if(opcode == OP_LD) {
        return 0;
    }

    return 0;
}
