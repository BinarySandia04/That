#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "toml.h"

namespace ZagIR {
class Package;

class PackCall {
public:
  PackCall();
  PackCall(std::string); 
  std::string funcName;
};

class Package {
public:
  Package(std::filesystem::path, toml::parse_result);

  // Class with all info
  std::string name;
  std::string version;
  std::string space;
  std::string root;
  std::string path;

  std::vector<std::string> fileDeps;

  std::unordered_map<std::string, PackCall> packMap;
private:
  void AddPackMapRecursive(std::string, std::unordered_map<std::string, PackCall>*, toml::table);
  bool EndsWith(std::string, std::string);
};

std::vector<Package> FetchPackages();
Package* FetchPackage(std::string);
}; // namespace ZagIR
