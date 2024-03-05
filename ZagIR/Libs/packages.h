#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "toml.h"

namespace ZagIR {

class Binding {
public:
  virtual ~Binding() = default;

  std::string name;
  std::string bind;

  bool global = false;
  bool good;
  bool duped;

  std::string foundBind;
  std::string realBind;

  std::string subpackage;
};

class CFunction : public Binding {
public:
  CFunction();
  CFunction(std::string);

  std::string retType;
  std::vector<std::string> funcArgs;
};

class CType : public Binding {
public:
  CType(std::string);

  std::string parent;
  std::string upgrades_to;
  std::vector<std::string> include;
  bool internal;
};

class Conversion : public Binding {
public:
  std::string lType;
  std::string rType;
  bool implicit;
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

  std::vector<std::string> subpackages;

  std::vector<Binding *> binds;

  // Ok si es prescindible el resultat d'això s'hauria de separar a un arxiu
  // Dins del mateix lloc que el .so
  void ComputeBinds();

  void LoadSubPackage(std::string subPackage);

private:
  toml::parse_result _res;

  void AddObjectsMap(std::string, toml::table, std::string);
  void AddTypeMap(std::string, toml::table, std::string);
  void AddConversionsMap(std::string, toml::table, std::string);
  bool EndsWith(std::string, std::string);
};

void FetchPackages(std::vector<Package *> *packages);
Package *FetchPackage(std::string);
}; // namespace ZagIR
