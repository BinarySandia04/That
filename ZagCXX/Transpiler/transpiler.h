#pragma once

#include <string>
#include <map>
#include <tuple>

#include <ZagIR/Ast/ast.h>

namespace ZagCXX {


class Transpiler {
  public:
    Transpiler();

    void AddInclude(std::string);
    void LoadLib(std::string);

    std::string ConvertType(std::string);
    std::string GenerateIncludes();
    std::string SanitizeIdentifier(std::string);

    std::string GenerateSource(ZagIR::Node*);

    std::string TranspileBlock(ZagIR::Node*);
    std::string TranspileStatement(ZagIR::Node*);
    std::string TranspileAssignation(ZagIR::Node*);
    std::string TranspileType(ZagIR::Node*);
    std::string TranspileIdentifier(ZagIR::Node*);
    std::string TranspileExpression(ZagIR::Node*);
    std::string TranspileBinary(ZagIR::Node*);
    std::string TranspileUnary(ZagIR::Node*);

    std::string TranspileIf(ZagIR::Node*);
    std::string TranspileLup(ZagIR::Node*);
    std::string TranspileGet(ZagIR::Node*);

  private:
    std::vector<std::string> includes;
    std::map<std::string, std::tuple<std::string, std::string>> typeMap;
    // std::vector<std::vector<VarObject>> stack;
};

}; // namespace ZagCXX
