#pragma once

#include <string>
#include <unordered_map>

#include <ZagIR/Libs/packages.h>

namespace ZagCXX {
enum ObjectType {
  OBJECT_EMPTY,
  OBJECT_VARIABLE,
  OBJECT_CONTAINER,
  OBJECT_FUNCTION,
  OBJECT_CFUNCTION,
  OBJECT_CCONTAINER,
};

class Object {
public:
  Object();
  Object(std::string);
  Object(ZagIR::PackCall);
  ObjectType objType;

  void AddChild(Object, std::string);
  void Print();
  Object* Get(std::string);

  ZagIR::PackCall GetCFunctionData();
private:
  // TODO: Puc fer un union ??? cada enum només vol un d'aquests
  std::string varData;

  std::unordered_map<std::string, Object> containerData;

  ZagIR::PackCall cfunctionData; 

  std::vector<Object> functionArgs;
  Object* functionReturn;
};

class Scope {
public:
  std::unordered_map<std::string, Object> data;
};

}; // namespace ZagCXX
