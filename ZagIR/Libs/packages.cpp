#include <stdexcept>
#ifndef ZAG_PATH
#define ZAG_PATH "/usr/local/lib/zag/"
#endif

#include "packages.h"

#include "Logs/logs.h"
#include "Utils/system.h"

#include <algorithm>
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
  _res = result;
  this->path = path;
  // std::cout << path.string() << std::endl;

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

  if (toml::array *arr = result["_info"]["_subpackages"].as_array()) {
    for (int i = 0; i < arr->size(); i++) {
      std::optional<std::string> str = (*arr).get(i)->value<std::string>();
      if (str.has_value()) {
        this->subpackages.push_back(*str);
      } else {
        Logs::Error("Error parsing package.toml");
      }
    }
  }

  // Ah pq no existeixen
  if (!!result["_"]) {
    if (!!result["_"]["_"])
      AddObjectsMap("", *result["_"]["_"].as_table(), "");
    if (!!result["_"]["_types"])
      AddTypeMap("", *result["_"]["_types"].as_table(), "");
    if (!!result["_"]["_conversions"])
      AddConversionsMap("", *result["_"]["_conversions"].as_table(), "");
    if (!!result["_"]["_operations"])
      AddOperationsMap("", *result["_"]["_operations"].as_table(), "");
  }
}

void Package::LoadSubPackage(std::string subpackage) {

  auto it = std::find(subpackages.begin(), subpackages.end(), subpackage);
  if (it != subpackages.end()) {
    // Split string in .
    auto subTable = _res["_"];

    std::string key = "";
    for (int i = 0; i < subpackage.size(); i++) {
      if (subpackage[i] == '.' || i == subpackage.size() - 1) {
        if (i == subpackage.size() - 1)
          key += subpackage[i];
        if (!!subTable[key]) {
          subTable = subTable[key];
          key = "";
        } else {
          throw std::runtime_error("Subpackage " + subpackage + " of " + name +
                                   " does not exist");
        }
      } else
        key += subpackage[i];
    }

    if (!!subTable["_"])
      AddObjectsMap("", *subTable["_"].as_table(), subpackage);
    if (!!subTable["_types"])
      AddTypeMap("", *subTable["_types"].as_table(), subpackage);
    if (!!subTable["_conversions"])
      AddConversionsMap("", *subTable["_conversions"].as_table(), subpackage);
    if (!!subTable["_operations"])
      AddOperationsMap("", *subTable["_operations"].as_table(), subpackage);

    // No hauria pero idk
    ComputeBinds();
    return;
  }
  throw std::runtime_error("Subpackage " + subpackage + "of " + name +
                           " does not exist");
}

Package::~Package() {
  for (int i = 0; i < binds.size(); i++) {
    // std::cout << binds[i]->name << std::endl;
    delete binds[i];
  }
}

void Package::AddObjectsMap(std::string rootName, toml::table table,
                            std::string subpackage) {

  bool hasBind = false;
  for (auto [k, v] : table) {
    std::string key = std::string(k.str());

    if (EndsWith(key, "_function")) {
      CFunction *function = new CFunction(rootName);

      function->package = this;
      function->subpackage = subpackage;

      toml::table t = *v.as_table();

      SetupBinding(function, t);

      std::optional<std::string> retType = t["return"].value<std::string>();

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

      AddObjectsMap(nextKey, *v.as_table(), subpackage);
    }
  }
}

void Package::AddTypeMap(std::string rootName, toml::table table,
                         std::string subpackage) {

  for (auto [k, v] : table) {
    std::string key = std::string(k.str());

    CType *type = new CType(key);
    type->package = this;
    type->subpackage = subpackage;
    type->name = key;

    toml::table t = *v.as_table();
    SetupBinding(type, t);

    std::optional<bool> global = t["global"].value<bool>();
    std::optional<std::string> parent = t["parent"].value<std::string>();
    std::optional<std::string> upgrades_to =
        t["upgrades_to"].value<std::string>();
    std::optional<int> templates = t["templates"].value<int>();

    if (parent.has_value())
      type->parent = *parent;

    if (global.has_value())
      type->global = *global;

    if (upgrades_to.has_value())
      type->upgrades_to = *upgrades_to;

    if (templates.has_value())
      type->templates = *templates;
    else
      type->templates = 0;

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

void Package::AddConversionsMap(std::string rootName, toml::table table,
                                std::string subpackage) {
  for (auto [k, v] : table) {
    std::string key = std::string(k.str());

    Conversion *conversion = new Conversion();
    conversion->package = this;
    conversion->subpackage = subpackage;
    conversion->name = key;

    toml::table t = *v.as_table();
    SetupBinding(conversion, t);

    std::optional<bool> implicit = t["implicit"].value<bool>();
    std::optional<std::string> from = t["from"].value<std::string>();
    std::optional<std::string> to = t["to"].value<std::string>();

    if (implicit.has_value())
      conversion->implicit = *implicit;
    if (from.has_value())
      conversion->fromType = *from;
    if (to.has_value())
      conversion->toType = *to;

    binds.push_back(conversion);
  }
}

void Package::AddOperationsMap(std::string rootName, toml::table table,
                               std::string subpackage) {
  for (auto [k, v] : table) {
    std::string key = std::string(k.str());

    COperation *operation = new COperation();
    operation->package = this;
    operation->subpackage = subpackage;
    operation->name = key;

    toml::table t = *v.as_table();
    SetupBinding(operation, t);

    std::optional<bool> implicit = t["implicit"].value<bool>();
    std::optional<std::string> ltype = t["ltype"].value<std::string>();
    std::optional<std::string> rtype = t["rtype"].value<std::string>();
    std::optional<std::string> op = t["op"].value<std::string>();
    std::optional<std::string> resType = t["return"].value<std::string>();

    if (implicit.has_value())
      operation->implicit = *implicit;
    if (ltype.has_value())
      operation->lType = *ltype;
    if (rtype.has_value())
      operation->rType = *rtype;
    if (op.has_value())
      operation->op = *op;
    if (resType.has_value())
      operation->resType = *resType;

    binds.push_back(operation);
  }
}

void Package::SetupBinding(Binding *b, toml::table t) {
  std::optional<std::string> bind = t["bind"].value<std::string>();
  if (bind.has_value())
    b->bind = *bind;

  if (toml::array *arr = t["headers"].as_array()) {
    for (int i = 0; i < arr->size(); i++) {
      std::optional<std::string> str = (*arr).get(i)->value<std::string>();
      if (str.has_value()) {
        b->headers.push_back(*str);
      } else {
        Logs::Error("Error parsing package.toml");
      }
    }
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

    CType *ctype = dynamic_cast<CType *>(binds[i]);
    if (ctype != nullptr) {
      binds[i]->good = true;
      continue;
    }

    Conversion *conversion = dynamic_cast<Conversion *>(binds[i]);
    if (conversion != nullptr) {
      if (conversion->implicit) {
        binds[i]->good = true;
        continue;
      }
    }

    COperation *coperation = dynamic_cast<COperation *>(binds[i]);
    if (coperation != nullptr) {
      if (coperation->implicit) {
        binds[i]->good = true;
        continue;
      }
    }

    int f = 0;
    for (int j = 0; j < mangles.size(); j++) {

      // std::cout << demangles[j] << " " << binds[i]->bind << std::endl;
      if (demangles[j].find(binds[i]->bind) != std::string::npos) {
        f++;

        std::string firstManglePart = "";
        for (int k = 0; k < demangles[j].size() && demangles[j][k] != '('; k++)
          firstManglePart += demangles[j][k];
        binds[i]->foundBind = firstManglePart;

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

CType::CType(std::string typeName) { this->name = typeName; }
