#include "vm.h"
#include "instruction.h"

#include "../flags.h"

#include <string>

#include <That/Ast/node.h>
#include <That/Ast/ast.h>

using namespace Thatc;
using namespace That;

VM::VM() {
}

VM::~VM() {
}

int VM::runCode(std::string code){
  Node *ast = new Node();
  GenerateAst(code, "", ast, programFlags & Flags::DEBUG);

  // ast->Debug(0);
  Bytecode bytecode;
  compileBytecode(ast, &bytecode);
  bytecode.print();
  Logs::Print("Running bytecode");
  int res = runBytecode(&bytecode);
  Logs::Printf("Code execution ended with code %d\n", res);
  return 0;
}

int VM::compileBytecode(Node *ast, Bytecode* bytecode){
  Constant* c = new Constant();
  c->type = ConstantType::T_INT;
  c->value.i = 3;
  bytecode->pushConstant(c);
  bytecode->pushInstruction(new Instruction(OpCode::OP_LOADK, 0, 0, 0));
  bytecode->pushInstruction(new Instruction(OpCode::OP_RET, 0, 0, 0));
  return 0;
}

int VM::runBytecode(Bytecode* bytecode){
  ip = 0;
  for(;;){
    Instruction* ins = bytecode->instructions[ip];
    switch(ins->getOpCode()){
      case OpCode::OP_RET:
        return 0;
      case OpCode::OP_LOADK:
        
      default:
        return 1;
    }
    ip++;
  }
  return 0;
}
