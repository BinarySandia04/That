#include "function.h"

#include "type.h"
#include "environment.h"

#include <ZagIR/Logs/logs.h>

#include <iostream>

using namespace ZagIR;
using namespace ZagCXX;

void ObjectFunction::Print(int space) {
  std::cout << "[ObjectFunction]" << std::endl;
}

Object* ObjectFunction::Clone(){
  ObjectFunction* copy = new ObjectFunction();
  copy->functionArgs = functionArgs;
  copy->returnType = returnType;
  copy->inheritedType = inheritedType;
  return copy;
}

void ObjectFunction::SetInheritedType(ObjectType *type) {
  this->inheritedType = type;
  for (int i = 0; i < functionArgs.size(); i++) {
    std::string arg = functionArgs[i];
    if (arg.size() == 0)
      continue; // ??? HUH
    if (arg[0] == '$') {
      try {
        int posArg = std::stoi(arg.substr(1, arg.size() - 1)) - 1;
        functionArgs[i] = type->children[posArg]->identifier;
      } catch (std::exception &ex) {
        Logs::Error(ex.what());
      }
    }
  }
}

std::string ObjectFunction::GetName() { return "_f_" + this->identifier; }

bool ObjectFunction::CheckArgs(std::vector<ObjectType *> &args,
                               Environment *env) {
  if (args.size() != functionArgs.size()) {
    return false;
  }
  for (int i = 0; i < args.size(); i++) {
    if (functionArgs[i] == "Any")
      continue;

    ObjectType *argType = env->FetchType(functionArgs[i]);
    if (!args[i]->AbstractedFrom(argType)) {
      Logs::Error(args[i]->identifier + " " + functionArgs[i]);
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

Object* ObjectCFunction::Clone(){
  return new ObjectCFunction(cFunctionData);
}

std::string ObjectCFunction::GetName() { return this->cFunctionData->bind; }

void ObjectNativeFunction::Print(int space) {
  std::cout << "[ObjectNativeFunction]" << std::endl;
}

Object* ObjectNativeFunction::Clone(){
  return new ObjectNativeFunction();
}

std::string ObjectNativeFunction::GetName() { return "_f_" + this->identifier; }


