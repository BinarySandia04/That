#pragma once

#include <vector>
#include "instruction.h"
#include "constant.h"

namespace Thatc {
  class Bytecode {
    public:
      Bytecode();
      ~Bytecode();
      void print();
      int pushInstruction(Instruction* instruction);
      int pushConstant(Constant* constant);
      std::vector<Instruction*> instructions;
      std::vector<Constant*> constants;
  };
};
