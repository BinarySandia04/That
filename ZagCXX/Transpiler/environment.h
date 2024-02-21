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
  OBJECT_PACKAGE,
};

class Object {
public:
  Object();
  Object(std::string);
  Object(ZagIR::PackCall);
  Object(ZagIR::Package*);
  ObjectType objType;

  void AddChild(Object, std::string);
  void Print();
  Object* GetObject(std::string);
  ZagIR::Package* GetPackage();

  ZagIR::PackCall GetCFunctionData();
private:
  // TODO: Puc fer un union ??? cada enum només vol un d'aquests
  std::string varData;

  std::unordered_map<std::string, Object> containerData;

  ZagIR::PackCall cfunctionData; 

  std::vector<Object> functionArgs;
  Object* functionReturn;

  ZagIR::Package* package;
};

class Scope {
public:
  std::unordered_map<std::string, Object> data;
};

}; // namespace ZagCXX
