#pragma once

#include <string>
#include <unordered_map>

#include <ZagIR/Libs/packages.h>

#include "vartype.h"

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
  Object(std::string, VarType*);
  Object(ZagIR::PackCall);
  Object(ZagIR::Package *);
  Object(ObjectType);

  ObjectType objType;

  void AddChild(Object, std::string);
  void Print();
  Object *GetObject(std::string);
  VarType *GetVarType();
  ZagIR::Package *GetPackage();

  ZagIR::PackCall GetCFunctionData();

private:
  // TODO: Puc fer un union ??? cada enum només vol un d'aquests
  // OBJECT_VARIABLE
  std::string varName;
  VarType* varType;
  
  // OBJECT_CONTAINER
  std::unordered_map<std::string, Object> containerData;

  // OBJECT_CFUNCTION
  ZagIR::PackCall cfunctionData;

  // OBJECT_FUNCTION
  std::vector<VarType> functionArgs;
  VarType *functionReturn;

  // OBJECT_PACKAGE
  ZagIR::Package *package;
};

} // namespace ZagCXX
