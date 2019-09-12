// opcodes.h


#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>

#define OP_C0 (0)
#define OP_C1 (1)
#define OP_C2 (2)

#define OP_C0_LW (0x2)
#define OP_C0_SW (0x6)


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