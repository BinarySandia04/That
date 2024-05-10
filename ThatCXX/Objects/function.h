#pragma once

#include "object.h"
#include "type.h"

using namespace ThatIR;

namespace ThatCXX {

class ObjectType;

class ObjectFunction : public Object {
public:
  void Print(int);
  Object* Clone();

  void SetInheritedType(ObjectType *);

  virtual std::string GetName();

  bool CheckArgs(std::vector<ObjectType *> &, Environment *env);

  std::vector<std::string> functionArgs;
  std::string returnType;
private:
  ObjectType *inheritedType;
};

class ObjectCFunction : public ObjectFunction {
public:
  std::string GetName();
  ObjectCFunction(CFunction *);
  void Print(int);
  Object* Clone();

  CFunction *cFunctionData;
};

class ObjectNativeFunction : public ObjectFunction {
public:
  std::string GetName();
  void Print(int);
  Object* Clone();
};


};
