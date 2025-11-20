#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "ctypes.h"

#include "toml++/toml.hpp"

namespace fs = std::filesystem;

namespace That {

struct PackageInfo {
  std::string name;
  std::string import_name;
  std::string display_name;
  std::string version;
  std::string root;

  fs::path path;
};

class Package {
public:
  Package(std::filesystem::path, toml::parse_result);
  Package(std::filesystem::path);
  ~Package();

  PackageInfo packInfo;
  // Class with all info

  std::vector<Binding *> binds;

  // Ok si es prescindible el resultat d'això s'hauria de separar a un arxiu
  // Dins del mateix lloc que el .so
  // void ComputeBinds();

  // Deprecated
  void ParsePackageDefinition(toml::table, std::string);

private:
  void AddObjectsMap(std::string, toml::table, std::string, std::vector<Binding*>*);
  void AddTypeMap(std::string, toml::table, std::string);
  void AddConversionsMap(std::string, toml::table, std::string);
  void AddOperationsMap(std::string, toml::table, std::string);

  bool EndsWith(std::string, std::string);

  void SetupBinding(Binding*, toml::table);
};

void FetchPackages(std::vector<Package *> *packages);
Package *FetchPackage(std::string);
}; // namespace ThatLib
