// decode.c

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "opcodes.h"
#include "decode.h"
#include "simulator.h"
#include "detangle.h"

int decode_rv32i(uint32_t instruction_word, instruction_rv32i_t* dest){
    if(dest == NULL){
        return -1;
    }

    uint8_t opcode_bits = instruction_word & (0x7F);
    if(opcode_bits != OP_LUI &&
       opcode_bits != OP_AUIPC &&
       opcode_bits != OP_JAL &&
       opcode_bits != OP_JALR &&
       opcode_bits != OP_BR &&
       opcode_bits != OP_LD &&
       opcode_bits != OP_ST &&
       opcode_bits != OP_IMM &&
       opcode_bits != OP_REG) {

        // Invalid opcode, or not supported yet
        fprintf(stderr, "Invalid opcode\n");
        return -1;
    }
    switch (opcode_bits)
    {
    case OP_LUI:
    case OP_AUIPC:
        dest->opcode = (opcode_rv32i_t)opcode_bits;
        dest->ins_type = u_type;
        dest->u_data.rd = (instruction_word >> 7) & 0x7;
        dest->u_data.imm32 = detangle_rv32i(instruction_word, u_type);
        break;
    case OP_JAL:
        dest->opcode = OP_JAL;
        dest->ins_type = j_type;
        break;
    
    default:
        break;
    }
    
    return 0;

    
}



int decode_compressed(uint16_t instruction_word, unpacked_rvc_t* dest){
    if(dest == NULL){
        return -1;
    }

    uint8_t op = instruction_word & 0x3;

    switch (op)
    {
    case OP_C0:
        return decode_C0(instruction_word, dest);
        break;

    case OP_C1:
        //return decode_C1(instruction_word, dest);
        break;
    
    case OP_C2:
        //return decode_C2(instruction_word, dest);
        break;
    
    default:
        break;
    }

    return 0;
}

uint16_t detangle_imm_load_store(uint16_t instruction_word){
    uint32_t uimm_2 = (instruction_word >> 6) & 0x1;
    uint32_t uimm_6 = (instruction_word >> 5) & 0x1;
    uint32_t uimm_5_3 = (instruction_word >> 10) & 0x7;

    return uimm_6 << 6 | uimm_5_3 << 5 | uimm_2 << 1;
}

int decode_C0(uint16_t instruction_word, unpacked_rvc_t* dest){
    uint8_t funct3 = (instruction_word >> 13) & 0x7;

    switch (funct3)
    {
    case OP_C0_LW: // 010, LOAD WORD
        dest->opcode.funct3 = OP_C0_LW;
        dest->data.CL_data.imm = detangle_imm_load_store(instruction_word);
        dest->data.CL_data.rd = (instruction_word >> 2) & 0x7;
        dest->data.CL_data.rs1 = (instruction_word >> 7) & 0x7;
        return 0;
        break;

    case OP_C0_SW: // 110, STORE WORD
        dest->opcode.funct3 = OP_C0_SW;
        dest->data.CS_data.imm = detangle_imm_load_store(instruction_word);
        dest->data.CS_data.rs2 = (instruction_word >> 2) & 0x7;
        dest->data.CS_data.rs1 = (instruction_word >> 7) & 0x7;
        return 0;
        break;
    
    default:
        break;
    }
    return 0;
}