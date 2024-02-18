#ifndef ZAG_PATH
#define ZAG_PATH "/usr/local/lib/zag/"
#endif

#include "packages.h"
#include "toml.h"

#include <filesystem>
#include <iostream>
#include <optional>
#include <vector>

using namespace ZagIR;

std::vector<Package> ZagIR::FetchPackages() {
  std::vector<Package> packages;

  for (auto &p : std::filesystem::directory_iterator(ZAG_PATH "packages")) {
    if (p.is_directory()) {
      if (std::filesystem::exists(p.path() / "package.toml")) {
        try {
          Package pack(p.path(),
                       toml::parse_file((p.path() / "package.toml").string()));
          packages.push_back(pack);
        } catch (int e) {
        }
      }
    }
  }

  return packages;
}

Package::Package(std::filesystem::path path, toml::parse_result result) {
  std::optional<std::string> name, version, space, root;
  name = result["info"]["name"].value<std::string>();
  version = result["info"]["version"].value<std::string>();
  space = result["info"]["namespace"].value<std::string>();
  root = result["info"]["root"].value<std::string>();

  if (name.has_value())
    this->name = *name;
  else
    throw(1);

  if (version.has_value())
    this->version = *version;
  else
    throw(2);

  if (space.has_value())
    this->space = *space;
  else
    throw(3);

  if (root.has_value())
    this->root = *root;
  else
    throw(4);

  for (auto [k, v] : *result["root"].as_table()) { 
    toml::table t = *v.as_table();
    std::string funcName = *t["funcName"].value<std::string>();
    std::vector<std::string> fileDeps;
    toml::array* arrFileDeps = t["fileDeps"].as_array();
    for(int i = 0; i < arrFileDeps->size(); i++){
      fileDeps.push_back(*(arrFileDeps)[i].value<std::string>());
    }

    PackCall pack(funcName, fileDeps);
    this->packMap.insert(std::make_pair(std::string(k.str()), pack));
  }

}

PackCall::PackCall(std::string funcName, std::vector<std::string> fileDeps){
  this->funcName = funcName;
  this->fileDeps = fileDeps;
}
