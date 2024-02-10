#include <iostream>
#include <string>

#include "ZagIR/Ast/ast.h"
#include "Ast/node.h"

int main(){
  std::cout << "Hello world!" << std::endl;

  ZagIR::Node* ast = new ZagIR::Node();
  ZagIR::GenerateAst("test = 3", "test.zag", ast);

  ast->Debug(0);
  return 0;
}
