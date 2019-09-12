// decode.h

#ifndef DECODE_H
#define DECODE_H

#include "opcodes.h"

int decode_compressed(uint16_t instruction_word, unpacked_rvc_t* dest);
int decode_C0(uint16_t instruction_word, unpacked_rvc_t* dest);

#endif