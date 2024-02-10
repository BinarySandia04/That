#pragma once

#include <string>

#include "Ast/node.h"
#include "Ast/parser.h"
#include "Ast/scanner.h"
#include "Ast/token.h"
#include "Frontend/error.h"

namespace ZagIR {
bool GenerateAst(std::string code, std::string fileName, Node *ast) {
  Error error;

  Scanner scanner = Scanner(code, fileName);
  std::vector<Token> tokens;

  if(!scanner.ScanTokens(&tokens, &error)){
    error.Print(code);
    return false;
  }

  Parser parser(fileName);
  if(!parser.GenerateAST(&tokens, &ast, &error)){
    error.Print(code);
    return false;
  }

  return true;
}
}; // namespace ZagIR
