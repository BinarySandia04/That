#pragma once

#include <map>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>

#include <ZagIR/Ast/ast.h>
#include <ZagIR/Libs/packages.h>

#include "Objects/object.h"
#include "Objects/container.h"
#include "Objects/type.h"
#include "Objects/prototype.h"
#include "Objects/function.h"
#include "Objects/variable.h"
#include "Objects/operation.h"

#include "environment.h"
#include "Formatter/formatter.h"

using namespace ZagIR;

namespace ZagCXX {

class Transpiler {
public:
  // Podriem moure el 100% de tot això a private
  Transpiler();
  ~Transpiler();

  std::string GenerateSource(Node *, std::string*, std::vector<std::string>*);
private:

  Environment* env;

  Package *LoadPackage(std::string);
  Package *LoadHeadlessPackage(std::string);
  Package *GetLoadedPackage(std::string);

  void ThrowError(Node *, std::string);

  std::string GenerateIncludes();

  std::string TranspileBlock(Node *); 
  std::string TranspileStatement(Node *);
  std::string TranspileAssignation(Node *, std::string *);

  std::string TranspileIdentifier(Node *, ObjectType **);
  std::string TranspileExpression(Node *, ObjectType **, std::string *);
  std::string TranspileBinary(Node *, ObjectType **, std::string *);
  std::string TranspileUnary(Node *, ObjectType **, std::string *);

  std::string TranspileIf(Node *, std::string *);
  std::string TranspileLup(Node *, std::string *);
  std::string TranspileGet(Node *);
  std::string TranspileReturn(Node *, std::string *);
  std::string TranspileBrk(Node *, std::string *);

  std::string TranspileInstruction(Node*, ObjectType**, ObjectContainer**, Scope*, std::string*);

  ObjectNativeFunction* TranspileFunction(Node *, std::string nameSpace, std::string*, std::string*, bool transpileContent);
  std::string TranspileKin(Node *, std::string *);

  std::string PreTranspileMethod(Node*, std::string, std::string*, Object**, std::string *);
  void PostTranspileMethod(Node*, std::string, std::string*);
  
  std::string TranspileArray(Node*, ObjectType**, std::string *);

  std::string functionDeclaration;
  std::string functionDefinition;

  std::string classDeclaration;
  std::string classDefinition;

  std::vector<std::string> includes;
  std::vector<Package *> loadedPackages;

  int currentFormat;
  std::unordered_map<int, std::string> formatList;
  std::string WriteFormat(std::string);
};

}; // namespace ZagCXX
