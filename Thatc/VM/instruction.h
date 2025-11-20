#pragma once

#include <cstdint>

#include "op_codes.h"


namespace Thatc {
class Instruction {
public:
  Instruction(OpCode op_code, uint32_t A, uint32_t B, uint32_t C);
  Instruction(OpCode op_code, uint32_t A, uint32_t BX);
  void print();
  OpCode getOpCode();
  unsigned int getA();
  unsigned int getB();
  unsigned int getC();
  unsigned int getBX();
private:
  uint32_t data;
};
};
