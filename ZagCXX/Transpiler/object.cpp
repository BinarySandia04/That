#include "object.h"

#include <iostream>
#include <string>

#include <ZagIR/Logs/logs.h>

using namespace ZagIR;
using namespace ZagCXX;

void Object::Print() { std::cout << "[Object]" << std::endl; }

void ObjectEmpty::Print() { std::cout << "[ObjectEmpty]" << std::endl; }

void ObjectVariable::Print() { std::cout << "[ObjectVariable]" << std::endl; }

ObjectVariable::ObjectVariable(ObjectType *type, std::string name) {
  this->type = type;
  this->name = name;
}

void ObjectVariable::SetType(ObjectType *type) { this->type = type; }

ObjectType *ObjectVariable::GetType() { return type; }

std::string ObjectVariable::Transpile() { return "_v_" + this->name; }

void ObjectContainer::Print() { std::cout << "[ObjectContainer]" << std::endl; }

void ObjectContainer::AddObject(Object *obj, std::string path) {
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
  // TODO: Canviar això perque es faci bé collons

  if (containerData.find(firstPart) == containerData.end()) {
    if (secondPart.empty())
      containerData[firstPart] = obj;
    else {
      ObjectContainer *subContainer =
          dynamic_cast<ObjectContainer *>(containerData[firstPart]);
      if (subContainer != nullptr)
        subContainer->AddObject(obj, secondPart);
      else
        Logs::Error("Error adding subcontainer");
    }
  } else {
    ObjectContainer *subContainer =
        dynamic_cast<ObjectContainer *>(containerData[firstPart]);
    if (subContainer != nullptr)
      subContainer->AddObject(obj, secondPart);
    else
      Logs::Error("Error adding subcontainer");
  }
}

void ObjectContainer::AddBinding(ZagIR::Binding *b){
    CType *ctype = dynamic_cast<CType *>(b);
    CFunction *cfunction = dynamic_cast<CFunction *>(b);
    Conversion *conversion = dynamic_cast<Conversion *>(b);

    if (ctype != nullptr) {
      AddObject(new ObjectCType(ctype), b->name);
    } else if (cfunction != nullptr) {
      AddObject(new ObjectCFunction(cfunction), b->name);
    } else if (conversion != nullptr) {
      AddObject(new ObjectConversion(conversion), b->name);
    }
}

Object *ObjectContainer::GetObject(std::string key) {
  return containerData[key];
}

void ObjectFunction::Print() { std::cout << "[ObjectFunction]" << std::endl; }

std::string ObjectFunction::GetName() { return "_f_" + this->identifier; }

bool ObjectFunction::CheckArgs(std::vector<ObjectType *> &args) {
  if (args.size() != functionArgs.size())
    return false;
  for (int i = 0; i < args.size(); i++) {
    if (!args[i]->Equals(functionArgs[i]))
      return false;
  }
  return true;
}

ObjectCFunction::ObjectCFunction(ZagIR::CFunction *cfunction) {
  this->cFunctionData = cfunction;
}

void ObjectCFunction::Print() { std::cout << "[ObjectCFunction]" << std::endl; }

std::string ObjectCFunction::GetName() { return "_fc_" + this->identifier; }

void ObjectNativeFunction::Print() {
  std::cout << "[ObjectNativeFunction]" << std::endl;
}

std::string ObjectNativeFunction::GetName() {
  return "_fn_" + this->identifier;
}

void ObjectNativeFunction::SetReturnType(ObjectType *type) {
  this->functionReturn = type;
}

ObjectType *ObjectFunction::GetReturnType() { return this->functionReturn; }

ObjectConversion::ObjectConversion(ZagIR::Conversion *conversion) {
  this->conversion = conversion;
}

void ObjectConversion::Print() {
  std::cout << "[ObjectConversion]" << std::endl;
}

void ObjectType::Print() { std::cout << "[ObjectType]" << std::endl; }

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
}

void ObjectCType::Print() {
  std::cout << "[ObjectType > ObjectCType]" << std::endl;
}

bool ObjectCType::Equals(ObjectType *other) {
  // TODO
  return other->identifier == identifier;
}

std::string ObjectCType::Transpile() { return translation; }

void ObjectNativeType::Print() {
  std::cout << "[ObjectType > ObjectNativeType]" << std::endl;
}

bool ObjectNativeType::Equals(ObjectType *other) {
  // TODO
  return other->identifier == identifier;
}

std::string ObjectNativeType::Transpile() {
  std::cout << "Transpiling ObjectNativeType" << std::endl;
  return "";
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
