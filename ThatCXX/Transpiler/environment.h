#pragma once

#include <string>
#include <vector>
#include <map>

#include <ThatLib/Ast/node.h>

#include "Objects/object.h"
#include "Objects/container.h"
#include "Objects/prototype.h"
#include "Objects/type.h"
#include "Objects/operation.h"
#include "Objects/function.h"

#include "scope.h"

using namespace ThatLib;
namespace fs = std::filesystem;

namespace ThatCXX {

class Environment {
public:
  Environment();
  ~Environment();

  void DumpEnvironment();

  void PushScope();
  void PopScope();
  int ScopeCount();

  void AddPackageToScope(ThatLib::Package *package);

  void AddToRoot(std::string, Object*);
  void AddToScope(std::string, Object*);
  void AddToReserved(std::string, Object*);

  void AddInclude(std::string);
  void AddInclude(fs::path);
  void AddFileDep(ThatLib::Package *package, std::string);

  std::string GetIncludes();
  std::string GetCXXArgs();
  void GetLibNames(std::vector<std::string> *);

  bool ExistsInScope(std::string);
  bool Exists(std::string);
  bool ExistsInReserved(std::string);
  
  bool IsConcretedFrom(ObjectType*, std::string); // Type from left is concreted from right

  Object *Fetch(std::string);
  Object *FetchRoot(std::string);
  ObjectProtoType *FetchProtoType(std::string);
  
  ObjectType *FetchType(std::string);
  ObjectType *FetchType(Node*);

  Object* FetchReserved(std::string);
  ObjectCOperation *FetchOperation(ObjectType*, ObjectType*);

  void Use(ObjectProtoType *proto);
  void Use(ObjectCOperation *operation);
  void Use(ObjectType *type);
  void Use(ObjectCFunction *cFunc);

  ObjectType* Construct(ObjectProtoType*, std::vector<ObjectType *>);
  Scope* GetTopScope();

private:
  std::string recurseGetType(Node* );
  ObjectType* FetchExistingType(std::string);

  std::vector<std::string> includes;
  std::vector<std::string> absoluteIncludes;
  std::vector<std::string> packageNames;

  std::string cxxargs;

  std::string includeGlob;
  std::vector<std::string> fileDeps;
  std::vector<Scope*> environment;
  Scope* reserved;
};

}; // namespace ThatCXX
