#include <stdexcept>
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
#include <string>
#include <vector>

// https://stackoverflow.com/questions/307765/how-do-i-check-if-an-objects-type-is-a-particular-subclass-in-c

using namespace ZagIR;
namespace fs = std::filesystem;

void ZagIR::FetchPackages(std::vector<Package *> *packages) {
  for (auto &p : fs::directory_iterator(ZAG_PATH "packages")) {
    if (p.is_directory()) {
      if (fs::exists(p.path() / "package.toml")) {
        try {
          Package *pack = new Package(
              p.path(), toml::parse_file((p.path() / "package.toml").string()));
          pack->ComputeBinds();
          packages->push_back(pack);
        } catch (int e) {
          throw std::runtime_error("Error parsing package.toml");
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
              packageToml["_info"]["_name"].value<std::string>();
          if (packName.has_value()) {
            if (*packName == name) {
              Package *pack = new Package(p.path(), packageToml);
              pack->ComputeBinds();
              return pack;
            }
          }
        } catch (int e) {
          throw std::runtime_error("Error parsing package.toml");
        }
      }
    }
  }
  throw std::runtime_error("Package " + name + " does not exist");
}

Package::Package(std::filesystem::path path, toml::parse_result result) {
  this->path = path;

  std::optional<std::string> name, display_name, version, space, root,
      file_deps, required;
  name = result["_info"]["_name"].value<std::string>();
  display_name = result["_info"]["_display_name"].value<std::string>();
  version = result["_info"]["_version"].value<std::string>();
  root = result["_info"]["_root"].value<std::string>();

  if (name.has_value())
    this->name = *name;
  if (display_name.has_value())
    this->display_name = *display_name;
  if (version.has_value())
    this->version = *version;
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

  AddObjectsMap("", *result["_"].as_table());
  AddTypeMap("", *result["_types"].as_table());
  AddConversionsMap("", *result["_conversions"].as_table());
}

Package::~Package() {
  for (int i = 0; i < binds.size(); i++)
    delete binds[i];
}

void Package::AddObjectsMap(std::string rootName, toml::table table) {

  bool hasBind = false;
  for (auto [k, v] : table) {
    std::string key = std::string(k.str());

    if (EndsWith(key, "_function")) {
      CFunction *function = new CFunction(rootName);
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

      binds.push_back(function);
    } else {
      std::string nextKey;
      if (rootName != "")
        nextKey = rootName + "." + key;
      else
        nextKey = key;

      AddObjectsMap(nextKey, *v.as_table());
    }
  }
}

void Package::AddTypeMap(std::string rootName, toml::table table) {

  for (auto [k, v] : table) {
    std::string key = std::string(k.str());

    CType *type = new CType(key);
    toml::table t = *v.as_table();

    std::optional<std::string> type_accessor = t["type"].value<std::string>();
    std::optional<std::string> bind = t["bind"].value<std::string>();
    std::optional<std::string> parent = t["parent"].value<std::string>();

    if (bind.has_value())
      type->bind = *bind;

    if (parent.has_value())
      type->parent = *parent;

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

    binds.push_back(type);
  }
}

void Package::AddConversionsMap(std::string rootName, toml::table table) {
  for (auto [k, v] : table) {
    std::string key = std::string(k.str());

    Conversion *conversion = new Conversion();
    conversion->name = key;

    toml::table t = *v.as_table();

    std::optional<std::string> bind = t["bind"].value<std::string>();
    std::optional<std::string> from = t["from"].value<std::string>();
    std::optional<std::string> to = t["to"].value<std::string>();

    if (bind.has_value())
      conversion->bind = *bind;
    if (from.has_value())
      conversion->lType = *from;
    if (to.has_value())
      conversion->rType = *to;

    binds.push_back(conversion);
  }
}

bool Package::EndsWith(std::string fullString, std::string ending) {
  if (ending.size() > fullString.size())
    return false;

  return fullString.compare(fullString.size() - ending.size(), ending.size(),
                            ending) == 0;
}

void Package::ComputeBinds() {
  fs::path path = this->path;
  std::vector<std::string> mangles, demangles;
  std::string nmm, nmdm;
  nmm = Utils::Exec("nm -gDjUv " +
                    (path / ("lib" + this->name + ".so")).string());
  nmdm = Utils::Exec("nm -gDjUvC " +
                     (path / ("lib" + this->name + ".so")).string());

  auto nmmss = std::stringstream(nmm);
  auto nmdmss = std::stringstream(nmdm);

  for (std::string line; std::getline(nmmss, line, '\n');)
    mangles.push_back(line);
  for (std::string line; std::getline(nmdmss, line, '\n');)
    demangles.push_back(line);

  for (int i = 0; i < binds.size(); i++) {
    binds[i]->good = false;
    binds[i]->duped = false;

    int f = 0;
    for (int j = 0; j < mangles.size(); j++) {

      // std::cout << demangles[j] << " " << binds[i]->bind << std::endl;
      if (demangles[j].find(binds[i]->bind) != std::string::npos) {
        f++;
        binds[i]->foundBind = demangles[j];
        binds[i]->realBind = mangles[j];
        if (f > 1) {
          binds[i]->good = false;
          binds[i]->duped = true;
          break;
        }
      }
    }

    if (f == 1) {
      binds[i]->good = true;
    }
  }
}

CFunction::CFunction() {}

CFunction::CFunction(std::string funcName) { this->name = funcName; }

CType::CType(std::string typeName) { this->typeName = typeName; }
