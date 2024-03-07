#pragma once

#include <string>
#include <vector>

#include "object.h"

namespace fs = std::filesystem;

namespace ZagCXX {

class Object;
class ObjectType;
class ObjectCOperation;

class Scope {
public:
  void Print();
  void Delete();
  std::unordered_map<std::string, Object*> data;
};

class Environment {
public:
  Environment();
  ~Environment();

  void DumpEnvironment();

  void PushScope();
  void PopScope();

  void AddPackageToScope(ZagIR::Package *package);
  void AddSubPackageToScope(ZagIR::Package *package, std::string subpackage);

  void AddToRoot(std::string, Object*);
  void AddToScope(std::string, Object*);
  void AddToReserved(std::string, Object*);

  void AddInclude(std::string);
  void AddInclude(fs::path);
  void AddFileDep(ZagIR::Package *package, std::string);

  std::string GetIncludes();
  std::string GetCXXArgs();

  bool ExistsInScope(std::string);
  bool Exists(std::string);

  Object *Fetch(std::string);
  Object *FetchRoot(std::string);
  ObjectType *FetchType(std::string);

  Object* FetchReserved(std::string);
  ObjectCOperation *FetchOperation(ObjectType*, ObjectType*);
private:
  std::vector<std::string> includes;
  std::vector<std::string> absoluteIncludes;

  std::string cxxargs;

  std::string includeGlob;
  std::vector<std::string> fileDeps;
  std::vector<Scope> environment;
  Scope reserved;
};

}; // namespace ZagCXX
