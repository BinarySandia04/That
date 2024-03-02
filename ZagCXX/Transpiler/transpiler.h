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
  ~Transpiler();

  std::string GenerateSource(ZagIR::Node *);
private:

  void AddInclude(std::string);
  ZagIR::Package* LoadPackage(std::string);

  void ThrowError(ZagIR::Node*, std::string);

  std::string GenerateIncludes();

  std::string TranspileBlock(ZagIR::Node *);
  std::string TranspileStatement(ZagIR::Node *);
  std::string TranspileAssignation(ZagIR::Node *);

  std::string TranspileIdentifier(ZagIR::Node *, ObjectType**);
  std::string TranspileExpression(ZagIR::Node *, ObjectType**);
  std::string TranspileBinary(ZagIR::Node *, ObjectType**);
  std::string TranspileUnary(ZagIR::Node *, ObjectType**);

  std::string TranspileIf(ZagIR::Node *);
  std::string TranspileLup(ZagIR::Node *);
  std::string TranspileGet(ZagIR::Node *);
  std::string TranspileReturn(ZagIR::Node *);
  
  std::string TranspileCall(ZagIR::Node *, ObjectType**);

  std::string TranspileGCall(ObjectFunction*, ZagIR::Node *);

  std::string TranspileGetter(ZagIR::Node *, ObjectType**);

  std::string TranspileFunction(ZagIR::Node *);

  void PushScope();
  void PopScope();

  void AddPackageToScope(ZagIR::Package *package);
  void AddToRoot(std::string, Object*);
  void AddToScope(std::string, Object*);
  void AddBindingToScope(ZagIR::Binding *);
  void AddCTypeToScope(ZagIR::CType*);
  void AddCFunctionToScope(ZagIR::CFunction*);
  void AddConversionToScope(ZagIR::Conversion*);
  
  bool ExistsInScope(std::string);
  bool ExistsInRootScope(std::string);
  bool ExistsInEnv(std::string);

  Object* FetchEnvironment(std::string);
  Object* FetchRootEnvironment(std::string);
  ObjectType* FetchType(std::string);

  Formatter formatter;

  std::string functionDeclaration;
  std::string functionDefinition;

  std::vector<std::string> includes;
  std::vector<ZagIR::Package*> loadedPackages;
  
  std::set<std::string> fileDeps;
  std::string GlobFileDeps();

  std::vector<Scope> environment;

  int currentFormat;
  std::unordered_map<int, std::string> formatList;
  std::string WriteFormat(std::string);
};

}; // namespace ZagCXX
