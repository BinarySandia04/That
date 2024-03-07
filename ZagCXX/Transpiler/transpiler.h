#pragma once

#include <map>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>

#include <ZagIR/Ast/ast.h>
#include <ZagIR/Libs/packages.h>

#include "environment.h"
#include "formatter.h"

namespace ZagCXX {

class Transpiler {
public:
  // Podriem moure el 100% de tot això a private
  Transpiler();
  ~Transpiler();

  std::string GenerateSource(ZagIR::Node *, std::string*);
private:

  Environment* env;

  ZagIR::Package *LoadPackage(std::string);
  void LoadSubPackage(ZagIR::Package *, std::string);
  ZagIR::Package *GetLoadedPackage(std::string);

  void ThrowError(ZagIR::Node *, std::string);

  std::string GenerateIncludes();

  std::string TranspileBlock(ZagIR::Node *);
  std::string TranspileStatement(ZagIR::Node *);
  std::string TranspileAssignation(ZagIR::Node *);

  std::string TranspileIdentifier(ZagIR::Node *, ObjectType **);
  std::string TranspileExpression(ZagIR::Node *, ObjectType **);
  std::string TranspileBinary(ZagIR::Node *, ObjectType **);
  std::string TranspileUnary(ZagIR::Node *, ObjectType **);

  std::string TranspileIf(ZagIR::Node *);
  std::string TranspileLup(ZagIR::Node *);
  std::string TranspileGet(ZagIR::Node *);
  std::string TranspileReturn(ZagIR::Node *);

  std::string TranspileCall(ZagIR::Node *, ObjectType **);

  std::string TranspileGCall(ObjectFunction *, ZagIR::Node *, ObjectType**);

  std::string TranspileGetter(ZagIR::Node *, ObjectType **);

  std::string TranspileFunction(ZagIR::Node *);

  std::string functionDeclaration;
  std::string functionDefinition;

  std::vector<std::string> includes;
  std::vector<ZagIR::Package *> loadedPackages;

  // std::set<std::string> fileDeps;
  // std::string GlobFileDeps();

  int currentFormat;
  std::unordered_map<int, std::string> formatList;
  std::string WriteFormat(std::string);
};

}; // namespace ZagCXX
