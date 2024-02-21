#pragma once

#include <map>
#include <unordered_map>
#include <string>
#include <tuple>
#include <set>

#include <ZagIR/Ast/ast.h>
#include <ZagIR/Libs/packages.h>

#include "environment.h"
#include "formatter.h"

namespace ZagCXX {

class Transpiler {
public:
  // Podriem moure el 100% de tot això a private
  Transpiler();

  std::string GenerateSource(ZagIR::Node *);
private:
  void AddInclude(std::string);
  void LoadLib(std::string);

  std::string TranspileType(std::string);
  std::string GenerateIncludes();
  std::string SanitizeIdentifier(std::string);

  std::string TranspileBlock(ZagIR::Node *);
  std::string TranspileStatement(ZagIR::Node *);
  std::string TranspileAssignation(ZagIR::Node *);
  std::string TranspileIdentifier(ZagIR::Node *);
  std::string TranspileExpression(ZagIR::Node *);
  std::string TranspileBinary(ZagIR::Node *);
  std::string TranspileUnary(ZagIR::Node *);

  std::string TranspileIf(ZagIR::Node *);
  std::string TranspileLup(ZagIR::Node *);
  std::string TranspileGet(ZagIR::Node *);
  std::string TranspileReturn(ZagIR::Node *);
  
  std::string TranspileCall(ZagIR::Node *);
  std::string TranspileGCall(std::string, ZagIR::Node *);

  std::string TranspileGetter(ZagIR::Node *);

  std::string TranspileFunction(ZagIR::Node *);

  void PushScope();
  void PopScope();

  void AddToScope(std::string, Object);
  
  bool ExistsInScope(std::string);
  bool ExistsInEnv(std::string);

  Object* FetchEnvironment(std::string);

  void AddPackageToScope(ZagIR::Package *package);

  Formatter formatter;

  std::string functionDeclaration;
  std::string functionDefinition;

  std::vector<std::string> includes;
  std::vector<ZagIR::Package> loadedPackages;
  
  std::set<std::string> fileDeps;
  std::string GlobFileDeps();

  std::map<std::string, std::tuple<std::string, std::string>> typeMap;
  std::vector<Scope> environment;

  int currentFormat;
  std::unordered_map<int, std::string> formatList;
  std::string WriteFormat(std::string);
};

}; // namespace ZagCXX
