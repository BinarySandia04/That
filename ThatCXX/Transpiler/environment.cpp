#include "environment.h"

#include <iostream>

#include "termcolor/termcolor.hpp"

#include <ThatIR/Logs/logs.h>
#include <algorithm>
#include <stdexcept>

using namespace ThatCXX;
using namespace ThatIR;

Environment::Environment() {
  reserved = new Scope();
  PushScope();
}

Environment::~Environment() {
  // std::cout << termcolor::red << ScopeCount() << termcolor::reset <<
  // std::endl;
  PopScope();
  delete reserved;
}

void Environment::DumpEnvironment() {
  std::cout << "-------------------- RESERVED: -------------------"
            << std::endl;
  reserved->Print();
  std::cout << std::endl;
  for (int i = 0; i < environment.size(); i++) {
    std::cout << "---------------------" + std::to_string(i) +
                     "--------------------"
              << std::endl;
    std::cout << std::endl;
    environment[i]->Print();
  }
  std::cout << "--------------------------------------------" << std::endl;
}
void Environment::PushScope() {
  // std::cout << termcolor::green << "Pushed scope" << termcolor::reset <<
  // std::endl;
  if (environment.size() > 0) {
    environment.push_back(new Scope(environment[environment.size() - 1]));
  } else
    environment.push_back(new Scope());
}

void Environment::PopScope() {
  delete environment.back();
  environment.pop_back();
}

int Environment::ScopeCount() { return environment.size(); }

void Environment::AddPackageToScope(ThatIR::Package *package) {
  ObjectContainer *packContainer = new ObjectContainer();
  packContainer->identifier = package->packInfo.root;

  for (int i = 0; i < package->binds.size(); i++) {
    Binding *b = package->binds[i];

    // Check if the binding goes to root or reserved
    COperation *coperation = dynamic_cast<COperation *>(b);
    if (coperation != nullptr) {
      AddToReserved("C_" + package->packInfo.name + "_" + coperation->name,
                    GetObjectFromBinding(b));
    } else {
      if (b->global){
        AddToRoot(b->name, GetObjectFromBinding(b));
      }
      else
        packContainer->AddBinding(b);
    }
  }


  if (package->packInfo.root != "") {
    if (Exists(package->packInfo.root)) {
      // Hem de fer merge dels dos ObjectContainer s
      ObjectContainer *oldContainer =
          dynamic_cast<ObjectContainer *>(FetchRoot(package->packInfo.root));
      if (oldContainer == nullptr)
        throw std::logic_error("Existing root is not a package");

      oldContainer->Merge(packContainer);
      delete packContainer;
    } else {
      AddToRoot(package->packInfo.root, packContainer);
    }
  } else
    delete packContainer;

  // Now we load package to compiler
  cxxargs += "-L" + package->packInfo.path.string() +
             " -Wl,-rpath=" + package->packInfo.path.string() + " -l" + package->packInfo.name +
             " ";
  packageNames.push_back(fs::path(package->packInfo.path.string()).filename());
}

void Environment::AddToRoot(std::string name, Object *obj) {
  // if (!ExistsInRootScope(name))
  environment[0]->AddObject(name, obj);
}

void Environment::AddToScope(std::string name, Object *obj) {
  environment[environment.size() - 1]->AddObject(name, obj);
}

void Environment::AddToReserved(std::string name, Object *obj) {
  reserved->AddObject(name, obj);
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

void Environment::GetLibNames(std::vector<std::string> *libNames) {
  *libNames = packageNames;
}

std::string Environment::GetCXXArgs() { return cxxargs; }

bool Environment::Exists(std::string key) {
  for (int i = environment.size() - 1; i >= 0; i--) {
    if (environment[i]->Exists(key)) {
      return true;
    }
  }
  return false;
}

bool Environment::ExistsInScope(std::string key) {
  return environment.back()->Exists(key);
}

bool Environment::ExistsInReserved(std::string key) {
  return reserved->Exists(key);
}

bool Environment::IsConcretedFrom(ObjectType *type, std::string from) {}

Object *Environment::Fetch(std::string key) {
  for (int i = environment.size() - 1; i >= 0; i--) {
    if (environment[i]->Exists(key)) {
      return environment[i]->GetObject(key);
    }
  }
  return nullptr;
}

Object *Environment::FetchRoot(std::string key) {
  if (environment[0]->Exists(key))
    return environment[0]->GetObject(key);
  return nullptr;
}

Object *Environment::FetchReserved(std::string key) {
  if (ExistsInReserved(key))
    return reserved->GetObject(key);
  else
    return nullptr;
}

ObjectProtoType *Environment::FetchProtoType(std::string key) {
  ObjectProtoType *proto = dynamic_cast<ObjectProtoType *>(FetchRoot(key));
  if (proto != nullptr) {
    Use(proto);
    // proto->Use(this);
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
      res += recurseGetType(type->children[i]);
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
    res = Construct(FetchProtoType(baseProtoType), {});
    return res;
  }

  i++;
  int stack = 0;
  std::string argProtoType = "";
  std::vector<ObjectType *> args;
  do {
    if (typeId[i] == '<') {
      stack++;
    }
    if (typeId[i] == '>') {
      stack--;
    }

    if ((typeId[i] == ',' && stack == 0) || stack == -1) {
      args.push_back(FetchType(argProtoType));
      argProtoType = "";
      i++;
    } else {
      argProtoType += typeId[i];
    }

    i++;
  } while (i < typeId.size() && stack > -1);

  res = Construct(FetchProtoType(baseProtoType), args);
  res->identifier = typeId;
  return res;
}

ObjectCOperation *Environment::FetchOperation(ObjectType *ltype,
                                              ObjectType *rtype) {
  for (auto &p : *reserved) {
    ObjectCOperation *operation = dynamic_cast<ObjectCOperation *>(p.second->object);
    if (operation != nullptr) {
      if (operation->cOperationData->lType == ltype->identifier &&
          operation->cOperationData->rType == rtype->identifier) {
        // u->Use(this);
        Use(operation);
        return operation;
      }
    }
  }
  return nullptr;
}

ObjectType *Environment::Construct(ObjectProtoType *proto,
                                   std::vector<ObjectType *> args) {
  // If we construct it we suppose that it doesnt exists
  // We check first that the number of args are the same

  ObjectType *constructed = new ObjectType();
  constructed->children = args;

  // TODO: No hi ha cTypeInfo
  constructed->identifier = proto->identifier;

  if (proto->cTypeInfo != nullptr)
    constructed->translation = proto->cTypeInfo->parent;
  else
    constructed->translation = "c_" + proto->identifier;

  constructed->constructor = proto;
  if (args.size() > 0) {
    constructed->identifier += "<";
    for (int i = 0; i < args.size(); i++) {
      constructed->identifier += args[i]->identifier;
      if (i < args.size() - 1)
        constructed->identifier += ",";
    }
    constructed->identifier += ">";
  }

  AddToReserved(constructed->identifier, constructed);
  return constructed;
}

Scope *Environment::GetTopScope() {
  return environment[environment.size() - 1];
}

void Environment::Use(ObjectProtoType *proto) {
  // Logs::Debug("Used");
  if (proto->cTypeInfo == nullptr) {
    return;
  }
  for (int i = 0; i < proto->cTypeInfo->headers.size(); i++) {
    fs::path filePath = fs::path("src") /
                        proto->cTypeInfo->package->packInfo.path.filename() /
                        proto->cTypeInfo->headers[i];
    AddInclude(filePath);
  }
  for (int i = 0; i < proto->cTypeInfo->include.size(); i++) {
    AddInclude(proto->cTypeInfo->include[i]);
  }
}

void Environment::Use(ObjectCOperation *op) {
  for (int i = 0; i < op->cOperationData->headers.size(); i++) {
    fs::path filePath = fs::path("src") /
                        op->cOperationData->package->packInfo.path.filename() /
                        op->cOperationData->headers[i];
    AddInclude(filePath);
  }
}

void Environment::Use(ObjectCFunction *cFunc) {
  for (int i = 0; i < cFunc->cFunctionData->headers.size(); i++) {
    fs::path filePath = fs::path("src") /
                        cFunc->cFunctionData->package->packInfo.path.filename() /
                        cFunc->cFunctionData->headers[i];
    AddInclude(filePath);
  }
}
