#include "instruction.h"

#include "op_codes.h"
#include <cstdint>
#include <iostream>

#include <That/logs.h>

using namespace Thatc;
using namespace That;

// A - B - C
// A - BX

const uint32_t OPCODE_MASK = 0x3F;
const uint32_t A_MASK = 0x3FC0;
const uint32_t B_MASK = 0x7FC000;
const uint32_t C_MASK = 0xFF800000;
const uint32_t BX_MASK = 0xFFFFC000;

Instruction::Instruction(OpCode op_code, uint32_t A, uint32_t B, uint32_t C){
  data = 0;
  data |= (op_code & OPCODE_MASK) | ((A << 6) & A_MASK) | ((B << 14) & B_MASK) | ((C << 23) & C_MASK);
}

Instruction::Instruction(OpCode op_code, uint32_t A, uint32_t BX){
  data |= (op_code & OPCODE_MASK) | ((A << 6) & A_MASK) | ((BX << 14) & BX_MASK);
}

void Instruction::print(){
  Logs::Printf("OP: %7s, A: %d, B: %d, C: %d", toString(getOpCode()), getA(), getB(), getC());
}

OpCode Instruction::getOpCode(){
  uint32_t value = data & OPCODE_MASK;
  return static_cast<OpCode>(value);
}

unsigned int Instruction::getA(){
  return static_cast<unsigned int>( (data & A_MASK) >> 6);
}


unsigned int Instruction::getB(){
  return static_cast<unsigned int>( (data & B_MASK) >> 14);
}


unsigned int Instruction::getC(){
  return static_cast<unsigned int>((data & C_MASK) >> 23);
}


unsigned int Instruction::getBX(){
  return static_cast<unsigned int>( (data & BX_MASK) >> 14 );
}


