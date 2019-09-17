// decode.c

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "opcodes.h"
#include "decode.h"
#include "simulator.h"
#include "detangle.h"

#define GET_RS1(x) (((x) >> 15) & 0x1F)
#define GET_RS2(x) (((x) >> 20) & 0x1F)
#define GET_RD(x)  (((x) >>  7) & 0x1F)
#define GET_FUNCT3(x) (((x) >> 12) & 0x7)

int decode_rv32i(uint32_t instruction_word, instruction_rv32i_t* dest){
    if(dest == NULL){
        return -1;
    }

    uint8_t opcode_bits = instruction_word & (0x7F);
    //printf("Opcode: 0x%02x - ", opcode_bits);


    switch (opcode_bits)
    {
    case OP_LUI:
    case OP_AUIPC:
        dest->opcode = (opcode_rv32i_t)opcode_bits;
        dest->ins_type = u_type;
        dest->u_data.rd = GET_RD(instruction_word);
        dest->u_data.imm32 = detangle_rv32i(instruction_word, u_type);
        break;
    case OP_JAL:
        dest->opcode = OP_JAL;
        dest->ins_type = j_type;
        dest->j_data.imm21 = detangle_rv32i(instruction_word, j_type);
        break;
    case OP_JALR:
        dest->opcode = OP_JALR;
        dest->ins_type = i_type;
        dest->i_data.imm12 = detangle_rv32i(instruction_word, i_type);
        dest->i_data.rs1 = GET_RS1(instruction_word);
        dest->i_data.rd = GET_RD(instruction_word);
        break;
    case OP_BR:
        dest->opcode = OP_BR;
        dest->ins_type = b_type;
        dest->b_data.funct3 = GET_FUNCT3(instruction_word);
        dest->b_data.imm13 = detangle_rv32i(instruction_word, b_type);
        dest->b_data.rs1 = GET_RS1(instruction_word);
        break;
    case OP_LD:
        dest->opcode = OP_LD;
        dest->ins_type = i_type;
        dest->i_data.funct3 = GET_FUNCT3(instruction_word);
        dest->i_data.imm12 = detangle_rv32i(instruction_word, i_type);
        dest->i_data.rs1 = GET_RS1(instruction_word);
        dest->i_data.rd = GET_RD(instruction_word);
        break;
    case OP_ST:
        dest->opcode = OP_ST;
        dest->ins_type = s_type;
        dest->s_data.funct3 = GET_FUNCT3(instruction_word);
        dest->s_data.rs1 = GET_RS1(instruction_word);
        dest->s_data.rs2 = GET_RS2(instruction_word);
        dest->s_data.imm12 = detangle_rv32i(instruction_word, s_type);
        break;
    case OP_IMM:
        dest->opcode = OP_IMM;
        dest->ins_type = i_type;
        dest->i_data.funct3 = GET_FUNCT3(instruction_word);
        dest->i_data.imm12 = detangle_rv32i(instruction_word, i_type);
        dest->i_data.rs1 = GET_RS1(instruction_word);
        dest->i_data.rd = GET_RD(instruction_word);
        break;
    case OP_REG:
        dest->opcode = OP_REG;
        dest->ins_type = r_type;
        dest->r_data.rd = GET_RD(instruction_word);
        dest->r_data.rs1 = GET_RS1(instruction_word);
        dest->r_data.rs2 = GET_RS2(instruction_word);
        dest->r_data.funct3 = GET_FUNCT3(instruction_word);
        dest->r_data.funct7 = (instruction_word >> 25) & 0x7F;
        break;
    default:
        //fprintf(stderr, "Invalid or unsupported opcode!");
        return -1;
    }
    
    return 0;
}

int pretty_print_rv32i(instruction_rv32i_t ins){
    switch (ins.opcode)
    {
    case OP_LUI:
        printf("lui ");
    case OP_AUIPC:
        printf("auipc ");
        break;
    case OP_JAL:
        printf("jal ");
        break;
    case OP_JALR:
        printf("jalr ");
        break;
    case OP_BR:
        printf("br ");
        break;
    case OP_LD:
        printf("ld ");
        break;
    case OP_ST:
        printf("st ");
        break;
    case OP_IMM:
        printf("imm ");
        break;
    case OP_REG:
        printf("reg ");
        break;
    default:
        //fprintf(stderr, "Invalid or unsupported opcode!");
        return -1;
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