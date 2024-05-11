#pragma once

#include "object.h"
#include "type.h"

using namespace ThatLib;

namespace ThatCXX {

class ObjectType;

class ObjectVariable : public Object {
public:
  void Print(int);
  Object* Clone();

  ObjectVariable(ObjectType *, std::string name);

  void SetType(ObjectType *);
  ObjectType *GetType();

  std::string Transpile();

private:
  std::string name;
  ObjectType *type;
};


};
