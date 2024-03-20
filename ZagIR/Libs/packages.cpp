#include "packages.h"
#include <stdexcept>

#include "Logs/logs.h"
#include "Utils/system.h"
#include "Utils/zagpath.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

using namespace ZagIR;
namespace fs = std::filesystem;

void ZagIR::FetchPackages(std::vector<Package *> *packages) {
  for (auto &p : fs::directory_iterator(ZAG_PATH / "sources")) {
    if (p.is_directory()) {
      if (fs::exists(p.path() / "package.toml")) {
        try {
          Package *pack = new Package(
              p.path(), toml::parse_file((p.path() / "package.toml").string()));
          packages->push_back(pack);
        } catch (int e) {
          throw std::runtime_error("Error parsing package.toml");
        }
      }
    }
  }
}

Package *ZagIR::FetchPackage(std::string name) {
  for (auto &p : std::filesystem::directory_iterator(ZAG_PATH / "sources")) {
    if (p.is_directory()) {
      if (std::filesystem::exists(p.path() / "package.toml")) {
        try {
          toml::parse_result packageToml =
              toml::parse_file((p.path() / "package.toml").string());
          std::optional<std::string> packName =
              packageToml["_info"]["_import_name"].value<std::string>();
          if (packName.has_value()) {
            if (*packName == name) {
              Package *pack = new Package(p.path(), packageToml);
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
      file_deps, required, import_name;
  name = result["_info"]["_name"].value<std::string>();
  display_name = result["_info"]["_display_name"].value<std::string>();
  import_name = result["_info"]["_import_name"].value<std::string>();
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
  if (import_name.has_value())
    this->import_name = *import_name;


  // Ah pq no existeixen TODO: ????
    ParsePackageDefinition(*result.as_table(), "");
}

void Package::ParsePackageDefinition(toml::table t, std::string subpac) {
  if (!!t["_"])
    AddObjectsMap("", *t["_"].as_table(), subpac, &binds);
  if (!!t["_types"])
    AddTypeMap("", *t["_types"].as_table(), subpac);
  if (!!t["_conversions"])
    AddConversionsMap("", *t["_conversions"].as_table(), subpac);
  if (!!t["_operations"])
    AddOperationsMap("", *t["_operations"].as_table(), subpac);
}

Package::~Package() {
  for (int i = 0; i < binds.size(); i++) {
    delete binds[i];
  }
}

void Package::AddObjectsMap(std::string rootName, toml::table table,
                            std::string subpackage,
                            std::vector<Binding *> *bindings) {
  // ????
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

      bindings->push_back(function);
    } else {
      std::string nextKey;
      if (rootName != "")
        nextKey = rootName + "." + key;
      else
        nextKey = key;

      AddObjectsMap(nextKey, *v.as_table(), subpackage, bindings);
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
    type->global = true;

    toml::table t = *v.as_table();
    SetupBinding(type, t);

    std::optional<std::string> parent = t["parent"].value<std::string>();
    std::optional<std::string> upgrades_to =
        t["upgrades_to"].value<std::string>();
    std::optional<int> templates = t["templates"].value<int>();

    if (parent.has_value())
      type->parent = *parent;

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

    std::vector<Binding *> functions;
    if (toml::table *methods = t["methods"].as_table()) {
      AddObjectsMap("", *methods, subpackage, &(type->children));
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
