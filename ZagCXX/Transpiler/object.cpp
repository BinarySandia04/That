#include "object.h"

#include <iostream>
#include <string>

#include <ZagIR/Logs/logs.h>

#include "termcolor.h"

using namespace ZagIR;
using namespace ZagCXX;

Object *ZagCXX::GetObjectFromBinding(Binding *b) {
  CType *ctype = dynamic_cast<CType *>(b);
  CFunction *cfunction = dynamic_cast<CFunction *>(b);
  Conversion *conversion = dynamic_cast<Conversion *>(b);
  COperation *coperation = dynamic_cast<COperation *>(b);

  if (ctype != nullptr) {
    return new ObjectCType(ctype);
  } else if (cfunction != nullptr) {
    return new ObjectCFunction(cfunction);
  } else if (conversion != nullptr) {
    return new ObjectConversion(conversion);
  } else if (coperation != nullptr){
    return new ObjectCOperation(coperation);
  }
  return new ObjectEmpty();
}

void Object::Print(int space) { std::cout << "[Object]" << std::endl; }

void Object::Use(Environment *t) {}

void ObjectEmpty::Print(int space) {
  std::cout << "[ObjectEmpty]" << std::endl;
}

void ObjectEmpty::Use(Environment *t){};

void ObjectVariable::Print(int space) {
  std::cout << "[ObjectVariable]" << std::endl;
}

void ObjectVariable::Use(Environment *t){};

ObjectVariable::ObjectVariable(ObjectType *type, std::string name) {
  this->type = type;
  this->name = name;
}

void ObjectVariable::SetType(ObjectType *type) { this->type = type; }

ObjectType *ObjectVariable::GetType() { return type; }

std::string ObjectVariable::Transpile() { return "_v_" + this->name; }

ObjectContainer::~ObjectContainer(){
  for(auto &p : containerData){
    delete p.second;
  }
}

void ObjectContainer::Print(int space) {
  std::cout << std::string(space, ' ') << "[ObjectContainer]" << std::endl;
  for (auto &p : containerData) {
    std::cout << std::string(space + 3, ' ') << termcolor::yellow << p.first
              << termcolor::reset << ": ";
    if (p.second != nullptr)
      p.second->Print(space + 3);
    else
      std::cout << "nullptr" << std::endl;
  }
}

void ObjectContainer::Use(Environment *t){};

void ObjectContainer::AddObject(Object *obj, std::string path) {
  // FirstPart: io
  // SecondPart: hola.tal.print
  std::string firstPart = "", secondPart = "";

  // Populate firstPart and secondPart
  for (int i = 0; i < path.size(); i++) {
    if (path[i] != '.') {
      firstPart += path[i];
    } else {
      i++;
      while (i < path.size()) {
        secondPart += path[i];
        i++;
      }
    }
  }

  if (secondPart == "") {
    containerData[firstPart] = obj;
  } else {
    ObjectContainer *subContainer;
    if (containerData.find(firstPart) != containerData.end()) {
      // Comprovem que es container
      subContainer = dynamic_cast<ObjectContainer *>(containerData[firstPart]);
      if (subContainer == nullptr) {
        std::cout << firstPart << "|" << secondPart << std::endl;
        Logs::Error("Tried to add object to an existing container");
        return;
      }
    } else {
      subContainer = new ObjectContainer();
      containerData[firstPart] = subContainer;
    }

    subContainer->AddObject(obj, secondPart);
  }
}

void ObjectContainer::AddBinding(ZagIR::Binding *b) {
  AddObject(GetObjectFromBinding(b), b->name);
}

Object *ObjectContainer::GetObject(std::string key) {
  return containerData[key];
}

void ObjectFunction::Print(int space) {
  std::cout << "[ObjectFunction]" << std::endl;
}

void ObjectFunction::Use(Environment *t){};

std::string ObjectFunction::GetName() { return "_f_" + this->identifier; }

bool ObjectFunction::CheckArgs(std::vector<ObjectType *> &args) {
  if (args.size() != functionArgs.size()) {
    return false;
  }
  for (int i = 0; i < args.size(); i++) {
    if (args[i]->identifier !=
        functionArgs[i]) // No m'agrada aquesta comparació
      return false;
  }
  return true;
}

ObjectCFunction::ObjectCFunction(ZagIR::CFunction *cfunction) {
  this->cFunctionData = cfunction;
  for (int i = 0; i < cfunction->funcArgs.size(); i++)
    this->functionArgs.push_back(cfunction->funcArgs[i]);
  this->returnType = cfunction->retType;
}

void ObjectCFunction::Print(int space) {
  std::cout << "[ObjectCFunction]" << std::endl;
}

void ObjectCFunction::Use(Environment *t) {
  for (int i = 0; i < cFunctionData->headers.size(); i++) {
    fs::path filePath =
        cFunctionData->package->path / "src" / cFunctionData->headers[i];
    t->AddInclude(filePath);
  }
}

std::string ObjectCFunction::GetName() {
  return this->cFunctionData->foundBind;
}

void ObjectNativeFunction::Print(int space) {
  std::cout << "[ObjectNativeFunction]" << std::endl;
}

void ObjectNativeFunction::Use(Environment *t){};

std::string ObjectNativeFunction::GetName() {
  return "_f_" + this->identifier;
}

ObjectConversion::ObjectConversion(ZagIR::Conversion *conversion) {
  this->conversion = conversion;
}

void ObjectConversion::Print(int space) {
  std::cout << "[ObjectConversion]" << std::endl;
}

void ObjectConversion::Use(Environment *t){};

void ObjectType::Print(int space) { std::cout << "[ObjectType]" << std::endl; }

void ObjectType::Use(Environment *t){};

bool ObjectType::Equals(ObjectType *other) {
  // TODO
  return other->identifier == identifier;
}

std::string ObjectType::Transpile() {
  std::cout << "Transpiling normal ObjectType" << std::endl;
  return "";
}

std::string ObjectType::TranspileChildren() {
  std::cout << "Transpile children" << std::endl;
  return "";
}

ObjectCType::ObjectCType(ZagIR::CType *type) {
  this->internal = type->internal;
  this->translation = type->parent;
  this->includes = type->include;
  this->upgrades_to = type->upgrades_to;
  this->identifier = type->name;
}

void ObjectCType::Print(int space) {
  std::cout << "[ObjectType > ObjectCType]" << std::endl;
}

void ObjectCType::Use(Environment *t) {
  for (int i = 0; i < includes.size(); i++)
    t->AddInclude(includes[i]);
};

bool ObjectCType::Equals(ObjectType *other) {
  // TODO
  return other->identifier == identifier;
}

std::string ObjectCType::Transpile() { return translation; }

void ObjectNativeType::Print(int space) {
  std::cout << "[ObjectType > ObjectNativeType]" << std::endl;
}

void ObjectNativeType::Use(Environment *t){};

bool ObjectNativeType::Equals(ObjectType *other) {
  // TODO
  return other->identifier == identifier;
}

std::string ObjectNativeType::Transpile() {
  std::cout << "Transpiling ObjectNativeType" << std::endl;
  return "";
}

ObjectCOperation::ObjectCOperation(COperation* operation){
  this->cOperationData = operation;
}

void ObjectCOperation::Print(int n){
  std::cout << "[ObjectCOperation]" << std::endl;
}

std::string ObjectCOperation::GetName(){
  return this->cOperationData->foundBind;
}

void ObjectCOperation::Use(Environment* t){
  for (int i = 0; i < cOperationData->headers.size(); i++) {
    fs::path filePath =
        cOperationData->package->path / "src" / cOperationData->headers[i];
    t->AddInclude(filePath);
  }
}

/*
VarType::VarType() {
  this->name = "Nil";
  this->translation = "void";
  this->includes = "";
}

VarType::VarType(std::string name, std::string translation,
                 std::string includes) {
  this->name = name;
  this->translation = translation;
  this->includes = includes;
}

std::string VarType::Transpile() {
  std::string formatted = "";

  for (int i = 0; i < translation.size(); i++) {
    if (i < translation.size() - 1) {
      switch (translation[i]) {
      case '{':
        i++;
        if (translation[i + 1] != '{') {
          formatted += TranspileChildren();
        }
        break;
      case '}':
        if (translation[i + 1] == '}') {
          i++;
        }
        break;
      default:
        break;
      }
    }
    formatted += translation[i];
  }

  return formatted;
}

std::string VarType::TranspileChildren(){
  std::string res = "";
  for(int i = 0; i < children.size(); i++){
    res += children[i]->Transpile();
    if(i < children.size() - 1) res += ",";
  }
  return res;
}
*/
