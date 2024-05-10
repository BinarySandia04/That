#include "type.h"

#include "prototype.h"

#include <iostream>

using namespace ThatIR;
using namespace ThatCXX;


void ObjectType::Print(int space) { std::cout << "[ObjectType]" << std::endl; }

Object* ObjectType::Clone(){
  ObjectType* newType = new ObjectType();
  newType->identifier = identifier;
  newType->translation = translation;
  newType->upgrades_to = upgrades_to;

  for(int i = 0; i < children.size(); i++) newType->children.push_back(dynamic_cast<ObjectType*>(children[i]->Clone()));
  newType->constructor = constructor;
  return newType;
}

Object* ObjectProtoType::Clone(){
  ObjectProtoType* newProtoType = new ObjectProtoType(cTypeInfo);
  newProtoType->typeMethods = dynamic_cast<ObjectContainer*>(typeMethods->Clone());
  return newProtoType;
}

bool ObjectType::Equals(ObjectType *other) { return this == other; }

bool ObjectType::AbstractedFrom(ObjectType *abstract) {
  if (abstract->constructor == constructor) {
    bool res = true;
    for (int i = 0; i < abstract->children.size(); i++)
      res = res && children[i]->AbstractedFrom(abstract->children[i]);
    return res;
  }
  return false;
}

std::string ObjectType::Transpile() {
  std::string t = translation;

  if (children.size() > 0) {
    t += "<";
    for (int i = 0; i < children.size(); i++)
      t += children[i]->Transpile();
    t += ">";
  }
  return t;
}

