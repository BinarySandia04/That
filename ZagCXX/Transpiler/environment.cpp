#include "environment.h"

#include "termcolor.h"

#include <algorithm>
#include <stdexcept>

using namespace ZagCXX;
using namespace ZagIR;

void Scope::Delete() {
  for (auto &p : data) {
    // std::cout << termcolor::yellow << p.first << termcolor::reset << ": ";
    if (p.second != nullptr) {
      /*
      std::cout << "Deleted ";
      p.second->Print();
      */
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

void Environment::DumpEnvironment() {
  for (int i = 0; i < environment.size(); i++) {
    std::cout << "--------------------------------------------" << std::endl;
    if (i == 0)
      std::cout << "Root:";
    else
      std::cout << i;
    std::cout << std::endl;
    environment[i].Print();
  }
  std::cout << "--------------------------------------------" << std::endl;
}
void Environment::PushScope() { environment.push_back(Scope()); }

void Environment::PopScope() {
  environment.back().Delete();
  environment.pop_back();
}

void Environment::AddPackageToScope(ZagIR::Package *package) {
  ObjectContainer *packContainer = new ObjectContainer();
  packContainer->identifier = package->name;

  for (int i = 0; i < package->binds.size(); i++) {
    Binding *b = package->binds[i];
    if (b->global)
      AddToRoot(b->name, GetObjectFromBinding(b));
    else
      packContainer->AddBinding(b);
  }

  AddToRoot(package->name, packContainer);

  // Now we load package to compiler
  cxxargs += "-L" + package->path.string() + " -Wl,-rpath=" + package->path.string() + " -l" + package->name + " ";
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

void Environment::AddInclude(std::string name) {
  if (std::find(includes.begin(), includes.end(), name) == includes.end()) {
    includes.push_back(name);
    includeGlob += "#include <" + name + ">\n";
  }
}

void Environment::AddInclude(fs::path p){
  if(std::find(absoluteIncludes.begin(), absoluteIncludes.end(), p.string()) == absoluteIncludes.end()){
    absoluteIncludes.push_back(p.string());
    includeGlob += "#include \"" + p.string() + "\"\n";
  }
}

std::string Environment::GetIncludes(){
  return includeGlob;
}

std::string Environment::GetCXXArgs(){
  return cxxargs;
}

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

Object *Environment::Fetch(std::string key) {
  for (int i = environment.size() - 1; i >= 0; i--) {
    if (environment[i].data.find(key) != environment[i].data.end()) {
      return environment[i].data[key];
    }
  }
  return nullptr;
}

Object *Environment::FetchRoot(std::string key) {
  return environment[0].data[key];
}

ObjectType *Environment::FetchType(std::string key) {
  // TODO: Suport per Type<> i que sigui safe?
  ObjectType* t = dynamic_cast<ObjectType *>(FetchRoot(key));
  if(t != nullptr){
    t->Use(this);
    return t;
  } else {
    throw std::runtime_error("Type " + key + " not found");
  }
}
