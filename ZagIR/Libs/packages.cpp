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

Package* ZagIR::FetchPackage(std::string name){
  for(auto &p : std::filesystem::directory_iterator(ZAG_PATH "packages")){
    if(p.is_directory()){
      if(std::filesystem::exists(p.path() / "package.toml")){
        try {
          toml::parse_result packageToml = toml::parse_file((p.path() / "package.toml").string());
          std::optional<std::string> packName = packageToml["info"]["_name"].value<std::string>();
          if(packName.has_value()){
            return new Package(p.path(), packageToml);
          }
        } catch(int e){}
      }
    }
  }
  std::cout << "No package found with that name" << std::endl;
}

Package::Package(std::filesystem::path path, toml::parse_result result) {
  std::optional<std::string> name, version, space, root;
  name = result["info"]["_name"].value<std::string>();
  version = result["info"]["_version"].value<std::string>();
  space = result["info"]["_namespace"].value<std::string>();
  root = result["info"]["_root"].value<std::string>();

  this->path = path.string();

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
  
  toml::array *arrFileDeps = result["info"]["_deps"].as_array();
  for (int i = 0; i < arrFileDeps->size(); i++) {
    fileDeps.push_back((std::string) * ((*arrFileDeps).get_as<std::string>(i)));
  }

  AddPackMapRecursive("", &packMap, *result["root"].as_table());
}

void Package::AddPackMapRecursive(
    std::string rootName, std::unordered_map<std::string, PackCall> *map,
    toml::table table) {

  std::string funcName;
  bool hasBind = false;
  for (auto [k, v] : table) {
    std::string key = std::string(k.str());
    // std::cout << rootName << std::endl;

    if (EndsWith(key, "_function")) {
      toml::table t = *v.as_table();
      std::optional<std::string> nameOpt;
      nameOpt = t["name"].value<std::string>();

      if(nameOpt.has_value()){
        funcName = *nameOpt;
      }
    }

    else {
      std::string nextKey;
      if (rootName != "")
        nextKey = rootName + "." + key;
      else
        nextKey = key;

      AddPackMapRecursive(nextKey, map, *v.as_table());
    }
  }

  if (hasBind) {
    PackCall pack(funcName);
    map->insert(std::make_pair(rootName, pack));
  }
}

bool Package::EndsWith(std::string fullString, std::string ending) {
  if (ending.size() > fullString.size())
    return false;

  return fullString.compare(fullString.size() - ending.size(), ending.size(),
                            ending) == 0;
}

PackCall::PackCall() {}

PackCall::PackCall(std::string funcName) {
  this->funcName = funcName;
}
