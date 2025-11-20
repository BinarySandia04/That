#pragma once

#include "op_codes.h"
#include "bytecode.h"

#include <string>
#include <That/Ast/node.h>

#include <That/Environment/environment.h>
// #include <ThatLib/Adapter/adapter.h>

using namespace That;

namespace Thatc {
  class VM {
    public:
      VM();
      ~VM();
      int runCode(std::string code);
      int compileBytecode(Node *ast, Bytecode *bytecode);
      int runBytecode(Bytecode* bytecode);
    private:
      int ip;
  };
};
