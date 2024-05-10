#include "variable.h"

#include <iostream>

using namespace ThatIR;
using namespace ThatCXX;


void ObjectVariable::Print(int space) {
  std::cout << "[ObjectVariable]" << std::endl;
}

Object* ObjectVariable::Clone(){
  return new ObjectVariable(type, name);
}

ObjectVariable::ObjectVariable(ObjectType *type, std::string name) {
  this->type = type;
  this->name = name;
}

void ObjectVariable::SetType(ObjectType *type) { this->type = type; }

ObjectType *ObjectVariable::GetType() { return type; }

std::string ObjectVariable::Transpile() { return "_v_" + this->name; }


