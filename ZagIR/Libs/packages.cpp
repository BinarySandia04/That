#ifndef ZAG_PATH
#define ZAG_PATH "/usr/local/lib/zag/"
#endif

#include "packages.h"
#include "toml.h"

#include "Logs/logs.h"
#include "Utils/system.h"

#include <filesystem>
#include <iostream>
#include <optional>
#include <vector>

using namespace ZagIR;
namespace fs = std::filesystem;

void ZagIR::FetchPackages(std::vector<Package*> *packages) {
  for (auto &p : fs::directory_iterator(ZAG_PATH "packages")) {
    if (p.is_directory()) {
      if (fs::exists(p.path() / "package.toml")) {
        try {
          Package* pack = new Package(p.path(),
                       toml::parse_file((p.path() / "package.toml").string()));
          pack->ComputeBinds(p.path());
          packages->push_back(pack);
        } catch (int e) {
        }
      }
    }
  }
}

Package *ZagIR::FetchPackage(std::string name) {
  for (auto &p : std::filesystem::directory_iterator(ZAG_PATH "packages")) {
    if (p.is_directory()) {
      if (std::filesystem::exists(p.path() / "package.toml")) {
        try {
          toml::parse_result packageToml =
              toml::parse_file((p.path() / "package.toml").string());
          std::optional<std::string> packName =
              packageToml["info"]["_name"].value<std::string>();
          if (packName.has_value()) {
            return new Package(p.path(), packageToml);
          }
        } catch (int e) {
        }
      }
    }
  }
  std::cout << "No package found with that name" << std::endl;
}

Package::Package(std::filesystem::path path, toml::parse_result result) {
  std::optional<std::string> name, display_name, version, space, root,
      file_deps, required;
  name = result["_info"]["_name"].value<std::string>();
  display_name = result["_info"]["_display_name"].value<std::string>();
  version = result["_info"]["_version"].value<std::string>();
  space = result["_info"]["_namespace"].value<std::string>();
  root = result["_info"]["_root"].value<std::string>();

  if (name.has_value())
    this->name = *name;
  if (display_name.has_value())
    this->display_name = *display_name;
  if (version.has_value())
    this->version = *version;
  if (space.has_value())
    this->space = *space;
  if (root.has_value())
    this->root = *root;

  if (toml::array *arr = result["_info"]["_file_deps"].as_array()) {
    for (int i = 0; i < arr->size(); i++) {
      std::optional<std::string> str = (*arr).get(i)->value<std::string>();
      if (str.has_value()) {
        this->file_deps.push_back(*str);
      } else {
        Logs::Error("Error parsing package.toml");
      }
    }
  }

  AddObjectsMap("", &functionMap, *result["_"].as_table());
  AddTypeMap   ("", &typeMap, *result["_types"].as_table());
}

Package::~Package(){
  for(auto &p : functionMap) delete p.second;
  for(auto &p : typeMap) delete p.second;
}

void Package::AddObjectsMap(std::string rootName,
                            std::unordered_map<std::string, FunctionCall *> *map,
                            toml::table table) {

  std::string function_bind;
  bool hasBind = false;
  for (auto [k, v] : table) {
    std::string key = std::string(k.str());

    if (EndsWith(key, "_function")) {
      FunctionCall* function = new FunctionCall(rootName);
      toml::table t = *v.as_table();

      std::optional<std::string> bind = t["bind"].value<std::string>();
      std::optional<std::string> retType = t["return"].value<std::string>();

      if (bind.has_value())
        function->bind = *bind;

      if (retType.has_value())
        function->retType = *retType;

      if (toml::array *arr = t["args"].as_array()) {
        for (int i = 0; i < arr->size(); i++) {
          std::optional<std::string> str = (*arr).get(i)->value<std::string>();
          if (str.has_value()) {
            function->funcArgs.push_back(*str);
          } else {
            Logs::Error("Error parsing package.toml");
          }
        }
      }

      map->insert(std::make_pair(rootName, function));
      binds.push_back(function);
    } else {
      std::string nextKey;
      if (rootName != "")
        nextKey = rootName + "." + key;
      else
        nextKey = key;

      AddObjectsMap(nextKey, map, *v.as_table());
    }
  }
}

void Package::AddTypeMap(std::string rootName,
                         std::unordered_map<std::string, ImportType *> *map,
                         toml::table table) {

  std::string function_bind;
  bool hasBind = false;
  for (auto [k, v] : table) {
    std::string key = std::string(k.str());

    ImportType* type = new ImportType(key);
    toml::table t = *v.as_table();

    std::optional<std::string> type_accessor = t["type"].value<std::string>();
    std::optional<std::string> bind = t["bind"].value<std::string>();

    if (bind.has_value())
      type->bind = *bind;

    if (type_accessor.has_value())
      type->typeAccessor = *type_accessor;

    if (toml::array *arr = t["include"].as_array()) {
      for (int i = 0; i < arr->size(); i++) {
        std::optional<std::string> str = (*arr).get(i)->value<std::string>();
        if (str.has_value()) {
          type->include.push_back(*str);
        } else {
          Logs::Error("Error parsing package.toml");
        }
      }
    }

    map->insert(std::make_pair(key, type));
    binds.push_back(type);
  }
}

bool Package::EndsWith(std::string fullString, std::string ending) {
  if (ending.size() > fullString.size())
    return false;

  return fullString.compare(fullString.size() - ending.size(), ending.size(),
                            ending) == 0;
}

void Package::ComputeBinds(fs::path path){
  std::cout << "nm -gDjUv " + (path / ("lib" + this->name + ".so") ).string() << std::endl;
  std::cout << Utils::Exec("nm -gDjUv " + (path / ("lib" + this->name + ".so") ).string()) << std::endl;
}


FunctionCall::FunctionCall() {}

FunctionCall::FunctionCall(std::string funcName) { this->name = funcName; }

ImportType::ImportType(std::string typeName) { this->typeName = typeName; }
