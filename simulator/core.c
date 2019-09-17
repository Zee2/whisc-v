// core.c

#include "core.h"
#include "decode.h"
#include "opcodes.h"
#include "simulator.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#define MEM_BOUNDS_CHECK(lower, upper, addr, width) \
    (((addr) + (width) - 1) > (upper) || ((addr) < (lower)))


// Forward decls of local functions

// Mutates regfile according to reg-reg instruction parameters
int execute_reg_reg(uint32_t instruction_bits, r_type_rv32i_t data, uint32_t* regfile);

int execute_imm_arith(uint32_t instruction_bits, i_type_rv32i_t data, uint32_t* regfile);

int execute_load(i_type_rv32i_t data, memory_t* memory, uint32_t* regfile);


// Fetches from memory, performs bounds check depending on "check"
// Fetches "width" bytes, in little-endian order
// Performs no sign extension
uint32_t fetch_width(memory_t* memory, uint32_t byte_addr, uint8_t width, uint8_t check){
    if(check == DO_BOUNDS_CHECK){
        if(MEM_BOUNDS_CHECK(memory->mem_lower_bound, memory->mem_upper_bound, byte_addr, width)){
            printf("Out of bounds memory access at address: %04x, width = %d", byte_addr, width);
            return 0xDEADBEEF; // addr out of bounds, helps prevent nasty VM escape loveliness :)
        }
    }
    uint32_t word = 0;
    for(int i = 0; i < width; i++){
        word |= memory->data[byte_addr + i] << (8*i);
    }

    return word;
    
}


int execute_rv32i(memory_t* memory, core_state_t* prev, core_state_t* next){
    if(prev == NULL || next == NULL || memory == NULL){
        return -1;
    }
    if(prev != next){
        memcpy(next, prev, sizeof(core_state_t));
    }

    // Fetch instruction from memory
    // fetch_width will perform bounds checking and frustrate
    // anyone trying to perform a VM escape
    uint32_t instruction_bits = fetch_width(memory, next->pc_reg, 4, DO_BOUNDS_CHECK);

    next->pc_reg += 4;

     // We set r0 to zero before executing the next instruction.
     // In hardware, r0 is wired directly to 0x0, but performing
     // checks on every instruction to do special behavior depending
     // on whether it uses x0 would introduce a large number of
     // unnecessary branches/checks. By setting it to zero first,
     // we effectively discard the x0 result of the last execution.
     // We will also set x0 = 0x0 after the execution is complete,
     // effectively discarding the x0 result of this execution.
    next->regfile[0] = 0;

    printf("Full instruction: %08x:  ", instruction_bits);

    // Decode the instruction
    instruction_rv32i_t decoded_ins;
    decode_rv32i(instruction_bits, &decoded_ins);

    printf("Opcode: 0x%02x - ", decoded_ins.opcode);

    // Each exec function will write to this as the return value
    int exec_result = 0; 

    // Execute the instruction
    switch (decoded_ins.opcode) {
    case OP_REG:
        // Thankfully, r_type instructions are only reg-reg instructions
        exec_result = execute_reg_reg(instruction_bits,
                                decoded_ins.r_data,
                                next->regfile);
        break;
    case OP_IMM:
        exec_result = execute_imm_arith(instruction_bits,
                                decoded_ins.i_data,
                                next->regfile);
        break;
    case OP_LD:
        exec_result = execute_load(decoded_ins.i_data,
                                memory,
                                next->regfile);
        break;
    default:
        printf(" NOP \n");
        break;
    }
    next->regfile[0] = 0;
    return exec_result;
}

int execute_reg_reg(uint32_t instruction_bits, r_type_rv32i_t data, uint32_t* regfile){
    switch(data.funct3)
    {
        // ADD/SUB
        case 0x0: 
        {
            
            int sign_bit = GET_MATH_BIT(instruction_bits) == 0 ? 1 : -1;
            if(sign_bit){
                printf("ADD");
            } else {
                printf("SUB");
            }
            regfile[data.rd] =
                (int32_t)regfile[data.rs1] + sign_bit * regfile[data.rs2];
            break;
        }
        // SLL (Shift Left Logical)
        case 0x1:
            printf("SLL");
            regfile[data.rd] =
                regfile[data.rs1] << regfile[data.rs2];
            break;
        // SLT (Set if Less Than)
        case 0x2:
            printf("SLT");
            regfile[data.rd] = 
                (int32_t)regfile[data.rs1] < (int32_t)regfile[data.rs2];
            break;
        // SLTU (Set if Less Than, Unsigned)
        case 0x3:
            printf("SLTU");
            regfile[data.rd] = 
                (uint32_t)regfile[data.rs1] < (uint32_t)regfile[data.rs2];
            break;
        // XOR
        case 0x4:
            printf("XOR");
            regfile[data.rd] = 
                regfile[data.rs1] ^ regfile[data.rs2];
            break;
        // SRL, SRA (Shift right, Logical or Arithmetic)
        case 0x5:
            // If bit30 is zero, it is a logical (unsigned) shift
            if(GET_MATH_BIT(instruction_bits) == 0){
                printf("SRL");
                regfile[data.rd] =
                    (uint32_t)regfile[data.rs1] >> (uint32_t)regfile[data.rs2];
            } else { // Else, arithmetic/signed shift
                printf("SRA");
                regfile[data.rd] =
                    (int32_t)regfile[data.rs1] >> (int32_t)regfile[data.rs2];
            }
            break;
        // OR
        case 0x6:
            printf("OR");
            regfile[data.rd] = 
                regfile[data.rs1] | regfile[data.rs2];
            break;
        // AND
        case 0x7:
            printf("AND");
            regfile[data.rd] = 
                regfile[data.rs1] & regfile[data.rs2];
            break;
        default:
            return -1;
    }
    printf(" - rd: x%d, rs1: x%d, rs2: x%d\n", data.rd, data.rs1, data.rs2);
    return 0;
}

int execute_imm_arith(uint32_t instruction_bits, i_type_rv32i_t data, uint32_t* regfile){
    switch (data.funct3) {
        // Sign-extended addition, immediate
        case IMM_ADDI:
            printf("ADDI");
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
            printf("Illegal immediate funct3 code: %01x\n", data.funct3);
            break;
    }
    printf(" - rd: x%d, rs1: x%d, imm: x%d\n", data.rd, data.rs1, SIGN_EXTEND(data.imm12, 12));
    return 0;
}

int execute_load(i_type_rv32i_t data, memory_t* memory, uint32_t* regfile){

    // Sign extend 12-bit immediate, add to rs1 base address
    uint32_t addr = (uint32_t)regfile[data.rs1] + SIGN_EXTEND(data.imm12, 12);
    
    uint32_t loaded_data;

    // Mask the lower two bits just to get the load width
    switch (data.funct3 & LD_WIDTH_MASK) {
        // Load byte
        case LD_B:
            // Fetch single byte from memory, request bounds check
            loaded_data = fetch_width(memory, addr, 1, DO_BOUNDS_CHECK);

            if(data.funct3 & LD_UNSIGNED_MASK){
                regfile[data.rd] = (uint32_t)loaded_data;
            } else {
                regfile[data.rd] = SIGN_EXTEND(loaded_data, 8);
            }

            break;

        // Load half
        case LD_H:
            if(MEM_BOUNDS_CHECK(memory->mem_lower_bound, memory->mem_upper_bound, addr, 2)){
                printf("Illegal memory access at %x", addr);
                return -1; // Requested memory out of bounds
            }
            // Fetch the two bytes from memory (little endian)
            uint32_t loaded_data = fetch_width(memory, addr, 2, DO_BOUNDS_CHECK);
            if(data.funct3 & LD_UNSIGNED_MASK){
                regfile[data.rd] = (uint32_t)loaded_data;
            } else {
                regfile[data.rd] = SIGN_EXTEND(loaded_data, 16);
            }
            break;

        // Load word
        case LD_W:
            if(MEM_BOUNDS_CHECK(memory->mem_lower_bound, memory->mem_upper_bound, addr, 2)){
                printf("Illegal memory access at %x", addr);
                return -1; // Requested memory out of bounds
            }
            // Fetch the four bytes from memory (little endian)
            regfile[data.rd] = fetch_width(memory, addr, 4, DO_BOUNDS_CHECK);
            break;
        default:
            printf("Invalid load width encoding: %x", data.funct3 & LD_WIDTH_MASK);
            return -1;
    }

    return 0;
    
}
