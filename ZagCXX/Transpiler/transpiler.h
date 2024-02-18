#pragma once

#include <map>
#include <string>
#include <tuple>

#include <ZagIR/Ast/ast.h>
#include <ZagIR/Libs/packages.h>

namespace ZagCXX {

enum ObjectType {
  OBJECT_EMPTY,
  OBJECT_VARIABLE,
  OBJECT_CONTAINER
};

class Object {
public:
  Object();
  Object(std::string);
  std::string type;
  ObjectType objType;
  std::unordered_map<std::string, Object> data;

  void AddPackageCall(std::string, ZagIR::PackCall);
};

class Scope {
  public:
  std::unordered_map<std::string, Object> data;
};

class Transpiler {
public:
  Transpiler();

  void AddInclude(std::string);
  void LoadLib(std::string);

  std::string ConvertType(std::string);
  std::string GenerateIncludes();
  std::string SanitizeIdentifier(std::string);

  std::string GenerateSource(ZagIR::Node *);

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

  void PushScope();
  void PopScope();

  void AddToScope(std::string, Object);
  bool ExistsInScope(std::string);
  Object* FetchEnvironment(std::string);

  void AddPackageToScope(ZagIR::Package package);
private:
  std::vector<std::string> includes;
  std::vector<ZagIR::Package> loadedPackages;
  std::map<std::string, std::tuple<std::string, std::string>> typeMap;
  std::vector<Scope> environment;
};

}; // namespace ZagCXX
