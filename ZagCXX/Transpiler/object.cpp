#include "object.h"

#include <iostream>
#include <string>

#include <ZagIR/Logs/logs.h>

using namespace ZagIR;
using namespace ZagCXX;

void Object::Print() { std::cout << "[Object]" << std::endl; }

void ObjectEmpty::Print() { std::cout << "[ObjectEmpty]" << std::endl; }

void ObjectVariable::Print() { std::cout << "[ObjectVariable]" << std::endl; }

ObjectVariable::ObjectVariable(std::string varName) { this->varName = varName; }

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

Object *ObjectContainer::GetObject(std::string key) { return containerData[key]; }

void ObjectFunction::Print() { std::cout << "[ObjectFunction]" << std::endl; }

void ObjectCFunction::Print() { std::cout << "[ObjectCFunction]" << std::endl; }

void ObjectConversion::Print() {
  std::cout << "[ObjectConversion]" << std::endl;
}

void ObjectCType::Print() { std::cout << "[ObjectCType]" << std::endl; }

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
