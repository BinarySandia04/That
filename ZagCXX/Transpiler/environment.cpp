#include "environment.h"

#include "termcolor/termcolor.hpp"

#include <ZagIR/Logs/logs.h>
#include <algorithm>
#include <stdexcept>

using namespace ZagCXX;
using namespace ZagIR;

void Scope::Delete() {
  for (auto &p : data) {
    // std::cout << termcolor::yellow << p.first << termcolor::reset << ": ";
    if (p.second != nullptr) {
      // std::cout << "Deleted ";
      // p.second->Print(0);
      delete p.second;
    }
  }
}

void Scope::Print() {
  for (auto &p : data) {
    std::cout << termcolor::yellow << p.first << termcolor::reset << ": ";
    if (p.second != nullptr)
      p.second->Print(0);
    else
      std::cout << "nullptr" << std::endl;
  }
}

Environment::Environment() { PushScope(); }

Environment::~Environment() {
  // std::cout << termcolor::red << ScopeCount() << termcolor::reset <<
  // std::endl;
  PopScope();
  reserved.Delete();
}

void Environment::DumpEnvironment() {
  std::cout << "-------------------- RESERVED: -------------------"
            << std::endl;
  reserved.Print();
  std::cout << std::endl;
  for (int i = 0; i < environment.size(); i++) {
    std::cout << "---------------------" + std::to_string(i) +
                     "--------------------"
              << std::endl;
    std::cout << std::endl;
    environment[i].Print();
  }
  std::cout << "--------------------------------------------" << std::endl;
}
void Environment::PushScope() {
  // std::cout << termcolor::green << "Pushed scope" << termcolor::reset <<
  // std::endl;
  environment.push_back(Scope());
}

void Environment::PopScope() {
  // std::cout << termcolor::red<< "Popped scope" << termcolor::reset <<
  // std::endl;
  environment.back().Delete();
  environment.pop_back();
}

int Environment::ScopeCount() { return environment.size(); }

void Environment::AddPackageToScope(ZagIR::Package *package) {
  ObjectContainer *packContainer = new ObjectContainer();
  packContainer->identifier = package->name;

  for (int i = 0; i < package->binds.size(); i++) {
    Binding *b = package->binds[i];

    // Check if the binding goes to root or reserved
    COperation *coperation = dynamic_cast<COperation *>(b);
    if (coperation != nullptr) {
      AddToReserved("C_" + package->name + "_" + coperation->name,
                    GetObjectFromBinding(b));
    } else {
      if (b->global)
        AddToRoot(b->name, GetObjectFromBinding(b));
      else
        packContainer->AddBinding(b);
    }
  }

  AddToRoot(package->name, packContainer);

  // Now we load package to compiler
  cxxargs += "-L" + package->path.string() +
             " -Wl,-rpath=" + package->path.string() + " -l" + package->name +
             " ";
  packageNames.push_back(fs::path(package->path.string()).filename());
}

void Environment::AddSubPackageToScope(ZagIR::Package *package,
                                       std::string subpackage) {
  ObjectContainer *packageContainer =
      dynamic_cast<ObjectContainer *>(Fetch(package->name));

  for (int i = 0; i < package->binds.size(); i++) {
    Binding *b = package->binds[i];
    if (b->subpackage == subpackage) {
      packageContainer->AddBinding(b);
    }
  }
}

void Environment::AddToRoot(std::string name, Object *obj) {
  // if (!ExistsInRootScope(name))
  environment[0].data.insert(std::pair<std::string, Object *>(name, obj));
}

void Environment::AddToScope(std::string name, Object *obj) {
  environment[environment.size() - 1].data.insert(
      std::pair<std::string, Object *>(name, obj));
}

void Environment::AddToReserved(std::string name, Object *obj) {
  reserved.data.insert(std::pair<std::string, Object *>(name, obj));
}

void Environment::AddInclude(std::string name) {
  if (std::find(includes.begin(), includes.end(), name) == includes.end()) {
    includes.push_back(name);
    includeGlob += "#include <" + name + ">\n";
  }
}

// Dep
void Environment::AddInclude(fs::path p) {
  if (std::find(absoluteIncludes.begin(), absoluteIncludes.end(), p.string()) ==
      absoluteIncludes.end()) {
    absoluteIncludes.push_back(p.string());
    includeGlob += "#include \"" + p.string() + "\"\n";
  }
}

// Dep
std::string Environment::GetIncludes() { return includeGlob; }

void Environment::GetLibNames(std::vector<std::string>* libNames){
  *libNames = packageNames;
}

std::string Environment::GetCXXArgs() { return cxxargs; }

bool Environment::Exists(std::string key) {
  for (int i = environment.size() - 1; i >= 0; i--) {
    if (environment[i].data.find(key) != environment[i].data.end()) {
      return true;
    }
  }
  return false;
}

bool Environment::ExistsInScope(std::string key) {
  return environment.back().data.find(key) != environment.back().data.end();
}

bool Environment::ExistsInReserved(std::string key) {
  return reserved.data.find(key) != reserved.data.end();
}

bool Environment::IsConcretedFrom(ObjectType* type, std::string from){
}

Object *Environment::Fetch(std::string key) {
  for (int i = environment.size() - 1; i >= 0; i--) {
    if (environment[i].data.find(key) != environment[i].data.end()) {
      return environment[i].data[key];
    }
  }
  return nullptr;
}

Object *Environment::FetchRoot(std::string key) {
  if (environment[0].data.find(key) != environment[0].data.end())
    return environment[0].data[key];
  return nullptr;
}

Object *Environment::FetchReserved(std::string key) {
  if (ExistsInReserved(key))
    return reserved.data[key];
  else
    return nullptr;
}

ObjectProtoType *Environment::FetchProtoType(std::string key) {
  ObjectProtoType *proto = dynamic_cast<ObjectProtoType *>(FetchRoot(key));
  if (proto != nullptr) {
    proto->Use(this);
    return proto;
  } else {
    throw std::runtime_error("ProtoType " + key + " not found");
  }
}

ObjectType *Environment::FetchExistingType(std::string key) {
  return dynamic_cast<ObjectType *>(FetchReserved(key));
}

ObjectType *Environment::FetchType(Node *type) {
  // Logs::Debug(recurseGetType(type));
  return FetchType(recurseGetType(type));
}

// Recurse get type
std::string Environment::recurseGetType(Node *type) {
  std::string res = type->data;
  if (type->children.size() > 0) {
    res += "<";
    for (int i = 0; i < type->children.size(); i++) {
      res += type->children[i]->data;
      if (i < type->children.size() - 1)
        res += ",";
    }
    res += ">";
  }
  return res;
}

ObjectType *Environment::FetchType(std::string typeId) {
  // TODO: Suport per Type<> i que sigui safe?
  // T<A1<...>,A2<...>,...,AN<...>>
  // Volem trobar el ProtoType T i després fetchejar recursivament els de dins

  ObjectType *res;
  res = FetchExistingType(typeId);
  if (res != nullptr)
    return res;

  std::string baseProtoType = "";
  int i = 0;
  while (i < typeId.size() && typeId[i] != '<') {
    baseProtoType += typeId[i];
    i++;
  }

  if (i >= typeId.size()) {
    // ProtoType terminal
    res = FetchProtoType(baseProtoType)->Construct({}, this);
    return res;
  }

  i++;
  int stack = 0;
  std::string argProtoType = "";
  std::vector<ObjectType *> args;
  do {
    if (typeId[i] == '<')
      stack++;
    if (typeId[i] == '>')
      stack--;

    if ((typeId[i] == ',' && stack == 0) || stack == -1) {
      args.push_back(FetchType(argProtoType));
      argProtoType = "";
      i++;
    } else {
      argProtoType += typeId[i];
    }

    i++;
  } while (i < typeId.size() && stack > -1);

  res = FetchProtoType(baseProtoType)->Construct(args, this);
  return res;
}

ObjectCOperation *Environment::FetchOperation(ObjectType *ltype,
                                              ObjectType *rtype) {
  for (auto &p : reserved.data) {
    ObjectCOperation *operation = dynamic_cast<ObjectCOperation *>(p.second);
    if (operation != nullptr) {
      if (operation->cOperationData->lType == ltype->identifier &&
          operation->cOperationData->rType == rtype->identifier) {
        Usable *u = dynamic_cast<Usable *>(operation);
        if (u != nullptr)
          u->Use(this);
        return operation;
      }
    }
  }
  return nullptr;
}
