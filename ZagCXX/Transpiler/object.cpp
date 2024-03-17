#include "object.h"

#include <iostream>
#include <stdexcept>
#include <string>

#include <ZagIR/Logs/logs.h>

#include "termcolor/termcolor.hpp"

using namespace ZagIR;
using namespace ZagCXX;

Object *ZagCXX::GetObjectFromBinding(Binding *b) {
  CType *ctype = dynamic_cast<CType *>(b);
  CFunction *cfunction = dynamic_cast<CFunction *>(b);
  Conversion *conversion = dynamic_cast<Conversion *>(b);
  COperation *coperation = dynamic_cast<COperation *>(b);

  if (ctype != nullptr) {
    return new ObjectProtoType(ctype);
  } else if (cfunction != nullptr) {
    return new ObjectCFunction(cfunction);
  } else if (conversion != nullptr) {
    return new ObjectConversion(conversion);
  } else if (coperation != nullptr) {
    return new ObjectCOperation(coperation);
  }
  return new ObjectEmpty();
}

void Object::Print(int space) { std::cout << "[Object]" << std::endl; }

void ObjectEmpty::Print(int space) {
  std::cout << "[ObjectEmpty]" << std::endl;
}

void ObjectVariable::Print(int space) {
  std::cout << "[ObjectVariable]" << std::endl;
}

ObjectVariable::ObjectVariable(ObjectType *type, std::string name) {
  this->type = type;
  this->name = name;
}

void ObjectVariable::SetType(ObjectType *type) { this->type = type; }

ObjectType *ObjectVariable::GetType() { return type; }

std::string ObjectVariable::Transpile() { return "_v_" + this->name; }

ObjectContainer::~ObjectContainer() {
  for (auto &p : containerData) {
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

std::string ObjectFunction::GetName() { return "_f_" + this->identifier; }

bool ObjectFunction::CheckArgs(std::vector<ObjectType *> &args) {
  if (args.size() != functionArgs.size()) {
    return false;
  }
  for (int i = 0; i < args.size(); i++) {
    if (args[i]->identifier !=
        functionArgs[i]) { // No m'agrada aquesta comparació
      std::cout << args[i]->identifier << " " << functionArgs[i] << std::endl;
      return false;
    }
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
        fs::path("src") / cFunctionData->package->path.filename() / cFunctionData->headers[i];
    t->AddInclude(filePath);
  }
}

std::string ObjectCFunction::GetName() {
  return this->cFunctionData->bind;
}

void ObjectNativeFunction::Print(int space) {
  std::cout << "[ObjectNativeFunction]" << std::endl;
}

std::string ObjectNativeFunction::GetName() { return "_f_" + this->identifier; }

ObjectConversion::ObjectConversion(ZagIR::Conversion *conversion) {
  this->conversion = conversion;
}

void ObjectConversion::Print(int space) {
  std::cout << "[ObjectConversion]" << std::endl;
}

ObjectProtoType::ObjectProtoType(CType *cTypeInfo) {
  this->cTypeInfo = cTypeInfo;
}

void ObjectProtoType::Print(int n) {
  std::cout << "[ObjectProtoType]" << std::endl;
}

void ObjectProtoType::Use(Environment *env) {
  // Logs::Debug("Used");
  for (int i = 0; i < cTypeInfo->headers.size(); i++) {
    fs::path filePath =
        cTypeInfo->package->path / "src" / cTypeInfo->headers[i];
    env->AddInclude(filePath);
  }
  for (int i = 0; i < cTypeInfo->include.size(); i++){
    env->AddInclude(cTypeInfo->include[i]);
  }
}

ObjectType *ObjectProtoType::Construct(std::vector<ObjectType *> args,
                                       Environment *env) {
  // If we construct it we suppose that it doesnt exists
  // We check first that the number of args are the same
  Use(env);
  if(args.size() != cTypeInfo->templates){
    throw std::runtime_error("Tried to construct type with an invalid number of arguments.");
    return nullptr;
  }

  ObjectType *constructed = new ObjectType();
  constructed->children = args;
  constructed->identifier = cTypeInfo->name;
  constructed->translation = cTypeInfo->parent;
  constructed->constructedBy = this;
  if (args.size() > 0) {
    constructed->identifier += "<";
    for (int i = 0; i < args.size(); i++) {
      constructed->identifier += args[i]->identifier;
      if (i < args.size() - 1)
        constructed->identifier += ",";
    }
    constructed->identifier += ">";
  }

  env->AddToReserved(constructed->identifier, constructed);
  return constructed;
}

void ObjectType::Print(int space) { std::cout << "[ObjectType]" << std::endl; }

bool ObjectType::Equals(ObjectType *other) {
  // TODO
  return other->identifier == identifier;
}

bool ObjectType::ConstructedBy(ObjectProtoType *protoType){
  return constructedBy == protoType && constructedBy != nullptr && protoType != nullptr;
}

std::string ObjectType::Transpile() {
  std::string t = translation;

  if (children.size() > 0) {
    t += "<";
    for (int i = 0; i < children.size(); i++) t += children[i]->Transpile();
    t += ">";
  }
  return t;
}

ObjectCOperation::ObjectCOperation(COperation *operation) {
  this->cOperationData = operation;
}

void ObjectCOperation::Print(int n) {
  std::cout << "[ObjectCOperation]" << std::endl;
}

std::string ObjectCOperation::GetName() {
  return this->cOperationData->foundBind;
}

void ObjectCOperation::Use(Environment *t) {
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
