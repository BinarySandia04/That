#pragma once

#include "object.h"
#include "prototype.h"

namespace ThatIR {
class ObjectProtoType;

class ObjectType : public Object {
public:
  void Print(int);
  Object* Clone();
  bool Equals(ObjectType *);
  bool AbstractedFrom(ObjectType *);

  std::string identifier;
  std::string translation;
  std::string upgrades_to;

  // ObjectType *parent;
  std::vector<ObjectType *> children;

  std::string Transpile();

  ObjectProtoType* constructor;
private:

  friend class ObjectProtoType;
};

};
