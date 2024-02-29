#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "toml.h"

namespace ZagIR {

class Binding {
public:
  std::string bind;
  bool good;
  std::string foundBind;
  std::string realBind;
};

class FunctionCall : public Binding {
public:
  FunctionCall();
  FunctionCall(std::string);

  std::string name;
  std::string retType;
  std::vector<std::string> funcArgs;
};

class ImportType : public Binding {
public:
  ImportType(std::string);

  std::string typeName;
  std::string parent;
  std::vector<std::string> include;
  std::string typeAccessor;
};

class Conversion : public Binding {
public:
  std::string name;
  std::string lType;
  std::string rType;
};

class Package {
public:
  Package(std::filesystem::path, toml::parse_result);
  ~Package();

  // Class with all info
  std::string name;
  std::string display_name;
  std::string version;
  std::string root;
  std::vector<std::string> file_deps;
  std::vector<std::string> include_directories;
  std::vector<std::string> link_directories;
  std::vector<std::string> link_libraries;

  std::string path;

  std::vector<std::string> fileDeps;

  std::vector<Binding *> binds;

  std::unordered_map<std::string, FunctionCall *> functionMap;
  std::unordered_map<std::string, ImportType *> typeMap;
  std::vector<Conversion *> conversionMap;

  // Ok si es prescindible el resultat d'això s'hauria de separar a un arxiu
  // Dins del mateix lloc que el .so
  void ComputeBinds(std::filesystem::path);

private:
  void AddObjectsMap(std::string,
                     std::unordered_map<std::string, FunctionCall *> *,
                     toml::table);
  void AddTypeMap(std::string, std::unordered_map<std::string, ImportType *> *,
                  toml::table);
  void AddConversionsMap(std::string, std::vector<Conversion* >*, toml::table);
  bool EndsWith(std::string, std::string);
};

void FetchPackages(std::vector<Package*> *packages);
Package *FetchPackage(std::string);
}; // namespace ZagIR
