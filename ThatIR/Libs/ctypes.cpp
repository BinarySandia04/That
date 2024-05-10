#include "ctypes.h"

using namespace ThatIR;

Binding::~Binding() {
  for (int i = 0; i < children.size(); i++) {
    delete children[i];
  }
}

Binding::Binding(Package *p){
  this->package = p;
}

CFunction::CFunction(Package *p, std::string funcName) : Binding(p) {
  this->name = funcName;
}

CType::CType(Package* p, std::string typeName) : Binding(p) {
  this->name = typeName;
}

Conversion::Conversion(Package *p) : Binding(p) {}

COperation::COperation(Package *p) : Binding(p) {}
