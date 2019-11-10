// decode.h

#ifndef DECODE_H
#define DECODE_H

#include "opcodes.h"

int decode_rv32i(uint32_t instruction_word, instruction_rv32i_t* dest);
int pretty_print_rv32i(instruction_rv32i_t ins, char* output);
int decode_compressed(uint16_t instruction_word, unpacked_rvc_t* dest);
int decode_C0(uint16_t instruction_word, unpacked_rvc_t* dest);

#endif