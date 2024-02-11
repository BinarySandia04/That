#pragma once

#include <string>
#include <map>
#include <tuple>

#include "ZagIR/Ast/ast.h"

namespace ZagCXX {

class Transpiler {
  public:
    Transpiler();

    void AddInclude(std::string);
    std::string ConvertType(std::string);
    std::string GenerateIncludes();

    std::string GenerateSource(ZagIR::Node*);

    std::string TranspileSpace(ZagIR::Node*);
    std::string TranspileStatement(ZagIR::Node*);
    std::string TranspileAssignation(ZagIR::Node*);
    std::string TranspileType(ZagIR::Node*);
    std::string TranspileIdentifier(ZagIR::Node*);
    std::string TranspileExpression(ZagIR::Node*);
    std::string TranspileBinary(ZagIR::Node*);
  private:
    std::vector<std::string> includes;
    std::map<std::string, std::tuple<std::string, std::string>> typeMap;
};

}; // namespace ZagCXX
