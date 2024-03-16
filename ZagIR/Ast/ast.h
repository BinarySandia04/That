#pragma once

#include <string>
#include <cstdint>

#include "error.h"
#include "node.h"
#include "parser.h"
#include "scanner.h"
#include "termcolor/termcolor.hpp"
#include "token.h"

namespace ZagIR {
bool GenerateAst(std::string code, std::string fileName, Node *ast, bool debug);
}; // namespace ZagIR
