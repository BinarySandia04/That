#pragma once

#include <string>
#include <cstdint>

#include "error.h"
#include "node.h"
#include "parser.h"
#include "scanner.h"
#include "token.h"
#include "Logs/logs.h"

namespace ThatLib {

  /*
   * Generates a node with the root containing the generated AST
   * -> Filename is needed for error handling in which file
   */
bool GenerateAst(std::string code, std::string fileName, Node *ast, bool debug);
}; // namespace ThatLib
