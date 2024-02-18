#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "toml.h"

namespace ZagIR {
class PackCall {
public:
  PackCall(std::string, std::vector<std::string>);
  std::string funcName;
  std::vector<std::string> fileDeps;
};

class Package {
public:
  Package(std::filesystem::path, toml::parse_result);

  // Class with all info
  std::string name;
  std::string version;
  std::string space;
  std::string root;

  std::unordered_map<std::string, PackCall> packMap;
};

std::vector<Package> FetchPackages();
}; // namespace ZagIR
