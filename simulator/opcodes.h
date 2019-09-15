// opcodes.h


#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>

#define OP_C0 (0)
#define OP_C1 (1)
#define OP_C2 (2)

#define OP_C0_LW (0x2)
#define OP_C0_SW (0x6)


//***************************************//
//             RV32I Metadata            //
//***************************************//

// Various 7-bit opcodes for RV32I
typedef enum opcode_rv32i_t
{
    OP_LUI = (0b0110111),
    OP_AUIPC = (0b0010111),
    OP_JAL = (0b1101111),
    OP_JALR = (0b1101011),
    OP_BR = (0b1100011),
    OP_LD = (0b0000011),
    OP_ST = (0b0100011),
    OP_IMM = (0b0010011),
    OP_REG = (0b0110011)
} opcode_rv32i_t;

// Instruction layout types for RV32I
typedef enum r_types_rv32i_t{
    r_type,
    i_type,
    s_type,
    b_type,
    u_type,
    j_type
} ins_types_rv32i_t;

// R-type RV32i instruction
typedef struct r_type_rv32i_t {
    uint8_t rd;
    uint8_t funct3;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t funct7;
} r_type_rv32i_t;

// I-type RV32i instruction, for immediate math and loading
typedef struct i_type_rv32i_t {
    uint8_t rd;
    uint8_t funct3;
    uint8_t rs1;
    uint16_t imm11_0;
} i_type_rv32i_t;

// S-type RV32i instruction, mostly for storing
typedef struct s_type_rv32i_t {
    uint8_t imm4_0;
    uint8_t funct3;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t imm11_5;
} s_type_rv32i_t;

// B-type RV32i instruction, mostly for branching
typedef struct b_type_rv32i_t {
    uint8_t imm4_1_11;
    uint8_t funct3;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t imm12_10_5;
} b_type_rv32i_t;

// U-type RV32i instruction, for the LUI/AUIPC instructions
typedef struct u_type_rv32i_t {
    uint8_t rd;
    uint32_t imm13_12;
} u_type_rv32i_t;

// J-type RV32i instruction, for the LUI/AUIPC instructions
typedef struct j_type_rv32i_t {
    uint8_t rd;
    uint32_t imm_tangled;
} j_type_rv32i_t;

// The overall instruction data for RV32I
typedef struct instruction_rv32i_t {
    ins_types_rv32i_t ins_type;
    opcode_rv32i_t opcode;
    union {
        r_type_rv32i_t r_data;
        i_type_rv32i_t i_data;
        s_type_rv32i_t s_data;
        b_type_rv32i_t b_data;
        u_type_rv32i_t u_data;
        j_type_rv32i_t j_data;
    } ins_data;
} instruction_rv32i_t;


//**************************************//
//             RV32C Below              //
//**************************************//


// Identifying features of a WHISC-V opcode
typedef struct opcode_rvc_t
{
    uint8_t op;     // 2 bits, not 00 (that would indicate a non-compressed instruction)
    uint8_t funct3; // 3 bits, MSB in the word
} opcode_rvc_t;

// Register type instruction
// C.MV and C.ADD use this.
typedef struct CR_type_t{
    uint8_t rs2;
    uint8_t rd_rs1;
    uint8_t extra_bit; // The LSB of the funct4 field
} CR_type_t;

// Register type instruction
// C.MV and C.ADD use this.
typedef struct CI_type_t{
    uint8_t imm5;
    uint8_t rd_rs1;
    uint8_t extra_bit; // The LSB of the funct4 field
} CI_type_t;

// Load type instruction
// 
typedef struct CL_type_t{
    uint8_t rd;
    uint8_t rs1;
    uint8_t imm;
} CL_type_t;

// Store type instruction
// 
typedef struct CS_type_t{
    uint8_t rs2;
    uint8_t rs1;
    uint8_t imm;
} CS_type_t;

// Arithmetic type instruction
// 
typedef struct CA_type_t{
    uint8_t rs2;
    uint8_t funct2;
    uint8_t rd_rs1;
    uint8_t funct2_2;
    uint8_t extra_bit; // Usually indicates SUBW/ADDW (unused)
} CA_type_t;

// Branch type instruction
// 
typedef struct CB_type_t{
    uint8_t offset5;
    uint8_t rs1;
    uint8_t offset3;
} CB_type_t;

// Branch type instruction
// 
typedef struct CJ_type_t{
    uint16_t jump_target_11; // Jump target, 11 bits
} CJ_type_t;

// A compressed J-type word in the RVC RISCV extension.
// Both JAL and J are examples of this word type.
// This would be implemented as a packed struct + 
// union, but such structures in C are not safe across
// multiple architectures.
// This struct also does not represent the bit structure
// of the actual instruction word. Consider this the
// "unpacked" instruction word.
typedef struct unpacked_rvc_t {
    opcode_rvc_t opcode;
    
    union
    {
        CR_type_t CR_data;
        CI_type_t CI_data;
        CL_type_t CL_data;
        CS_type_t CS_data;
        CA_type_t CA_data;
        CB_type_t CB_data;
        CJ_type_t CJ_data;

    } data;
    

} unpacked_rvc_t;



#endif