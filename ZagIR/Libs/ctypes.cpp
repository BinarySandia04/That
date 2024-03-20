#include "ctypes.h"

using namespace ZagIR;

Binding::~Binding(){
  for(int i = 0; i < children.size(); i++){
    delete children[i];
  }
}

CFunction::CFunction() {}

CFunction::CFunction(std::string funcName) { this->name = funcName; }

CType::CType(std::string typeName) { this->name = typeName; }
