#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <map>

namespace fs = std::filesystem;

namespace ThatLib {

class Package;

class Binding {
public:
  virtual ~Binding();
  Binding(Package*);

  Package *package;

  std::string name;
  std::string bind;

  bool global = false;

  std::string subpackage;
  std::vector<std::string> headers;

  std::vector<Binding *> children;
};

class CType : public Binding {
public:
  CType(Package*, std::string);

  std::string parent;
  std::string upgrades_to;
  std::map<std::string, std::string> accessor_map;
  std::vector<std::string> include;

  int templates = 0;
};

class CFunction : public Binding {
public:
  CFunction(Package*, std::string);

  std::string retType;
  std::vector<std::string> funcArgs;
};

class Conversion : public Binding {
public:
  Conversion(Package*);
  std::string fromType;
  std::string toType;
  bool implicit;
};

class COperation : public Binding {
public:
  COperation(Package*);
  std::string lType;
  std::string rType;
  std::string resType;
  std::string op;
  bool implicit;
};

}; // namespace ThatLib
