#pragma once

#include <string>
#include <cstdint>

#include "error.h"
#include "node.h"
#include "parser.h"
#include "scanner.h"
#include "token.h"
#include "logs.h"

namespace That {

  /*
   * Generates a node with the root containing the generated AST
   * -> Filename is needed for error handling in which file
   */
bool GenerateAst(std::string code, std::string fileName, Node *ast, bool debug);
}; // namespace ThatLib
