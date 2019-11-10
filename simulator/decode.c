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
        dest->j_data.rd = GET_RD(instruction_word);
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
        dest->b_data.rs2 = GET_RS2(instruction_word);
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
        dest->r_data.math_bit = GET_MATH_BIT(instruction_word);
        break;
    default:
        //fprintf(stderr, "Invalid or unsupported opcode!");
        return -1;
    }
    
    return 0;
}

int pretty_print_rv32i(instruction_rv32i_t ins, char* output){

    int charcount = 0;
    char branch_code[5];

    switch (ins.opcode)
    {
    case OP_LUI:
        charcount += snprintf(output, 100, "LUI x%d, 0x%X", ins.u_data.rd, ins.u_data.imm32 >> 12);
        break;
    case OP_AUIPC:
        charcount += snprintf(output, 100, "AUIPC x%d, 0x%X", ins.u_data.rd, ins.u_data.imm32 >> 12);
        break;
    case OP_JAL:
        charcount += snprintf(output, 100, "JAL x%d, 0x%X", ins.j_data.rd, ins.j_data.imm21);
        break;
    case OP_JALR:
        charcount += snprintf(output, 100, "JALR x%d, 0x%X(x%d)", ins.i_data.rd, ins.i_data.imm12, ins.i_data.rs1);
        break;
    case OP_BR:
        switch(ins.b_data.funct3){
            case BR_BEQ:
                charcount += snprintf(output, 100, "BEQ x%d, x%d, 0x%X", ins.b_data.rs1, ins.b_data.rs2, ins.b_data.imm13);
                break;
            case BR_BNE:
                charcount += snprintf(output, 100, "BNE x%d, x%d, 0x%X", ins.b_data.rs1, ins.b_data.rs2, ins.b_data.imm13);
                break;
            case BR_BLT:
                charcount += snprintf(output, 100, "BLT x%d, x%d, 0x%X", ins.b_data.rs1, ins.b_data.rs2, ins.b_data.imm13);
                break;
            case BR_BGE:
                charcount += snprintf(output, 100, "BGE x%d, x%d, 0x%X", ins.b_data.rs1, ins.b_data.rs2, ins.b_data.imm13);
                break;
            case BR_BLTU:
                charcount += snprintf(output, 100, "BLTU x%d, x%d, 0x%X", ins.b_data.rs1, ins.b_data.rs2, ins.b_data.imm13);
                break;
            case BR_BGEU:
                charcount += snprintf(output, 100, "BGEU x%d, x%d, 0x%X", ins.b_data.rs1, ins.b_data.rs2, ins.b_data.imm13);
                break;
        }
        break;
    case OP_LD:
        switch(LD_WIDTH_MASK & ins.i_data.funct3){
            case LD_B:
                if(LD_UNSIGNED_MASK & ins.i_data.funct3)
                    charcount += snprintf(output, 100, "LBU x%d, 0x%X(x%d)", ins.i_data.rd, ins.i_data.imm12, ins.i_data.rs1);
                else
                    charcount += snprintf(output, 100, "LB x%d, 0x%X(x%d)", ins.i_data.rd, ins.i_data.imm12, ins.i_data.rs1);
                break;
            case LD_H:
                if(LD_UNSIGNED_MASK & ins.i_data.funct3)
                    charcount += snprintf(output, 100, "LHU x%d, 0x%X(x%d)", ins.i_data.rd, ins.i_data.imm12, ins.i_data.rs1);
                else
                    charcount += snprintf(output, 100, "LH x%d, 0x%X(x%d)", ins.i_data.rd, ins.i_data.imm12, ins.i_data.rs1);
                break;
            case LD_W:
                charcount += snprintf(output, 100, "LW x%d, 0x%X(x%d)", ins.i_data.rd, ins.i_data.imm12, ins.i_data.rs1);
                break;
        }
        break;
    case OP_ST:
        switch(LD_WIDTH_MASK & ins.s_data.funct3){
            case LD_B:
                charcount += snprintf(output, 100, "SB x%d, 0x%X(x%d)", ins.s_data.rs2, ins.s_data.imm12, ins.s_data.rs1);
                break;
            case LD_H:
                charcount += snprintf(output, 100, "SH x%d, 0x%X(x%d)", ins.s_data.rs2, ins.s_data.imm12, ins.s_data.rs1);
                break;
            case LD_W:
                charcount += snprintf(output, 100, "SW x%d, 0x%X(x%d)", ins.s_data.rs2, ins.s_data.imm12, ins.s_data.rs1);
                break;
        }
        break;
    case OP_IMM:
        switch(ins.i_data.funct3){
            case IMM_ADDI:
                charcount += snprintf(output, 100, "ADDI x%d, x%d, 0x%X", ins.i_data.rd, ins.i_data.rs1, ins.i_data.imm12);
                break;
            case IMM_SLTI:
                charcount += snprintf(output, 100, "SLTI x%d, x%d, 0x%X", ins.i_data.rd, ins.i_data.rs1, ins.i_data.imm12);
                break;
            case IMM_SLTIU:
                charcount += snprintf(output, 100, "SLTIU x%d, x%d, 0x%X", ins.i_data.rd, ins.i_data.rs1, ins.i_data.imm12);
                break;
            case IMM_XORI:
                charcount += snprintf(output, 100, "XORI x%d, x%d, 0x%X", ins.i_data.rd, ins.i_data.rs1, ins.i_data.imm12);
                break;
            case IMM_ORI:
                charcount += snprintf(output, 100, "ORI x%d, x%d, 0x%X", ins.i_data.rd, ins.i_data.rs1, ins.i_data.imm12);
                break;
            case IMM_ANDI:
                charcount += snprintf(output, 100, "ANDI x%d, x%d, 0x%X", ins.i_data.rd, ins.i_data.rs1, ins.i_data.imm12);
                break;
        }
        break;
    case OP_REG:
        switch(ins.r_data.funct3){
            case RR_ADDSUB:
                if(ins.r_data.math_bit)
                    charcount += snprintf(output, 100, "SUB x%d, x%d, x%d", ins.r_data.rd, ins.r_data.rs1, ins.r_data.rs2);
                else
                    charcount += snprintf(output, 100, "ADD x%d, x%d, x%d", ins.r_data.rd, ins.r_data.rs1, ins.r_data.rs2);
                break;
            case RR_SLL:
                charcount += snprintf(output, 100, "SUB x%d, x%d, x%d", ins.r_data.rd, ins.r_data.rs1, ins.r_data.rs2);
                break;
            case RR_SLT:
                charcount += snprintf(output, 100, "SUB x%d, x%d, x%d", ins.r_data.rd, ins.r_data.rs1, ins.r_data.rs2);
                break;
            case RR_SLTU:
                charcount += snprintf(output, 100, "SUB x%d, x%d, x%d", ins.r_data.rd, ins.r_data.rs1, ins.r_data.rs2);
                break;
            case RR_XOR:
                charcount += snprintf(output, 100, "SUB x%d, x%d, x%d", ins.r_data.rd, ins.r_data.rs1, ins.r_data.rs2);
                break;
            case RR_SR:
                if(ins.r_data.math_bit)
                    charcount += snprintf(output, 100, "SRA x%d, x%d, x%d", ins.r_data.rd, ins.r_data.rs1, ins.r_data.rs2);
                else
                    charcount += snprintf(output, 100, "SRL x%d, x%d, x%d", ins.r_data.rd, ins.r_data.rs1, ins.r_data.rs2);
                break;
            case RR_OR:
                charcount += snprintf(output, 100, "SUB x%d, x%d, x%d", ins.r_data.rd, ins.r_data.rs1, ins.r_data.rs2);
                break;
            case RR_AND:
                charcount += snprintf(output, 100, "SUB x%d, x%d, x%d", ins.r_data.rd, ins.r_data.rs1, ins.r_data.rs2);
                break;
        }
        break;
    }
    if(charcount == 0){
        charcount += snprintf(output, 100, "Invalid");
    }
    return charcount;
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