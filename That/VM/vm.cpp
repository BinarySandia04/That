#include "vm.h"

#include "../flags.h"

#include <string>

#include <ThatLib/Ast/node.h>
#include <ThatLib/Ast/ast.h>

using namespace That;

VM::VM() {
  env = new Environment(); 
}

VM::~VM() {
  delete env;
}

void VM::Run(std::string code){
  ThatLib::Node *ast = new ThatLib::Node();
  ThatLib::GenerateAst(code, "", ast, programFlags & Flags::DEBUG);
}
