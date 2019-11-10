// opcodes.h


#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>

#define OP_C0 (0)
#define OP_C1 (1)
#define OP_C2 (2)

#define OP_C0_LW (0x2)
#define OP_C0_SW (0x6)

// Some truly horrendous bit hackery
// to extract various bits and sign
// extend arbitrary bit vectors
// http://graphics.stanford.edu/~seander/bithacks.html#FixedSignExtend
#define GET_MATH_BIT(x) ((x >> 30) & 0x1)
#define GET_SIGN_BITMASK(b) (1U << ((b)-1))
#define SIGN_EXTEND(x, b) (((x) ^ GET_SIGN_BITMASK(b)) - GET_SIGN_BITMASK(b))


//***************************************//
//             RV32I Metadata            //
//***************************************//

// Various 7-bit opcodes for RV32I
typedef enum opcode_rv32i_t
{
    OP_LUI = (0b0110111),   // U-type
    OP_AUIPC = (0x17), // U-type
    OP_JAL = (0b1101111),
    OP_JALR = (0b1100111),
    OP_BR = (0b1100011),
    OP_LD = (0x3),
    OP_ST = (0b0100011),
    OP_IMM = (0x13),
    OP_REG = (0x33)
} opcode_rv32i_t;

// RV32i branch funct3 codes
typedef enum branch_types_rv32i_t
{
    BR_BEQ =  0x0,
    BR_BNE =  0x1,
    BR_BLT =  0x4,
    BR_BGE =  0x5,
    BR_BLTU = 0x6,
    BR_BGEU = 0x7
} branch_types_rv32i_t;

// RV32i immediate arithmetic funct3 encoding
typedef enum imm_arith_rv32i_t
{
    IMM_ADDI  = 0x0,
    IMM_SLTI  = 0x2,
    IMM_SLTIU = 0x3,
    IMM_XORI  = 0x4,
    IMM_ORI   = 0x6,
    IMM_ANDI  = 0x7
} imm_arith_rv32i_t;

// RV32i reg-reg arithmetic funct3 encoding
typedef enum regreg_arith_rv32i_t
{
    RR_ADDSUB = 0x0,
    RR_SLL    = 0x1,
    RR_SLT    = 0x2,
    RR_SLTU   = 0x3,
    RR_XOR    = 0x4,
    RR_SR     = 0x5,
    RR_OR     = 0x6,
    RR_AND    = 0x7
} regreg_arith_rv32i_t;

// RV32i load type funct3 encoding
typedef enum load_type_rv32i_t
{
    LD_B             = 0x0,
    LD_H             = 0x1,
    LD_W             = 0x2,
    LD_WIDTH_MASK    = 0x3,
    LD_UNSIGNED_MASK = 0x4
    // Used to extract the bit which determines
    // whether it is a signed/unsigned load
} load_type_rv32i_t;

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
    uint8_t math_bit;
} r_type_rv32i_t;

// I-type RV32i instruction, for immediate math and loading
typedef struct i_type_rv32i_t {
    uint8_t rd;
    uint8_t funct3;
    uint8_t rs1;
    uint16_t imm12;
} i_type_rv32i_t;

// S-type RV32i instruction, mostly for storing
typedef struct s_type_rv32i_t {
    uint8_t funct3;
    uint8_t rs1;
    uint8_t rs2;
    uint16_t imm12;
} s_type_rv32i_t;

// B-type RV32i instruction, mostly for branching
typedef struct b_type_rv32i_t {
    uint8_t funct3;
    uint8_t rs1;
    uint8_t rs2;
    uint16_t imm13;
} b_type_rv32i_t;

// U-type RV32i instruction, for the LUI/AUIPC instructions
typedef struct u_type_rv32i_t {
    uint8_t rd;
    uint32_t imm32;
} u_type_rv32i_t;

// J-type RV32i instruction, for the JAL instruction
typedef struct j_type_rv32i_t {
    uint8_t rd;
    uint32_t imm21;
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
    };
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