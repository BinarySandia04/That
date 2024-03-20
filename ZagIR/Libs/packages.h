#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "ctypes.h"

#include "toml++/toml.hpp"

namespace fs = std::filesystem;

namespace ZagIR {

class Package {
public:
  Package(std::filesystem::path, toml::parse_result);
  ~Package();

  // Class with all info
  std::string name;
  std::string import_name;
  std::string display_name;
  std::string version;
  std::string root;
  // std::vector<std::string> file_deps;
  std::vector<std::string> include_directories;
  std::vector<std::string> link_directories;
  std::vector<std::string> link_libraries;

  fs::path path;

  std::vector<std::string> fileDeps;

  std::vector<Binding *> binds;

  // Ok si es prescindible el resultat d'això s'hauria de separar a un arxiu
  // Dins del mateix lloc que el .so
  // void ComputeBinds();

  void ParsePackageDefinition(toml::table, std::string);

private:
  toml::parse_result _res;

  void AddObjectsMap(std::string, toml::table, std::string, std::vector<Binding*>*);
  void AddTypeMap(std::string, toml::table, std::string);
  void AddConversionsMap(std::string, toml::table, std::string);
  void AddOperationsMap(std::string, toml::table, std::string);

  bool EndsWith(std::string, std::string);

  void SetupBinding(Binding*, toml::table);
};

void FetchPackages(std::vector<Package *> *packages);
Package *FetchPackage(std::string);
}; // namespace ZagIR
