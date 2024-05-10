#pragma once

#include "object.h"
#include "type.h"
#include "container.h"

using namespace ThatIR;

namespace ThatCXX {

class Environment;
class ObjectType;
// Objecte que crea classes
class ObjectProtoType : public Object {
public:
  ObjectProtoType(ObjectContainer*, std::string);
  ObjectProtoType(CType *);
  ~ObjectProtoType();

  void Print(int);
  Object* Clone();
  // ObjectType *Construct(std::vector<ObjectType *>, Environment *);

  ObjectContainer* typeMethods;

  // Com que els tipus de classe són purament C, els abstraim
  // des d'aqui
  CType* cTypeInfo;
};


};
