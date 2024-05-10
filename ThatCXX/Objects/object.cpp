#include "object.h"

#include "prototype.h"
#include "function.h"
#include "operation.h"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include <ThatIR/Logs/logs.h>

#include "termcolor/termcolor.hpp"

using namespace ThatIR;
using namespace ThatCXX;

Object *ThatCXX::GetObjectFromBinding(Binding *b) {
  CType *ctype = dynamic_cast<CType *>(b);
  CFunction *cfunction = dynamic_cast<CFunction *>(b);
  Conversion *conversion = dynamic_cast<Conversion *>(b);
  COperation *coperation = dynamic_cast<COperation *>(b);

  if (ctype != nullptr) {
    return new ObjectProtoType(ctype);
  } else if (cfunction != nullptr) {
    return new ObjectCFunction(cfunction);
  } else if (conversion != nullptr) {
    return new ObjectConversion(conversion);
  } else if (coperation != nullptr) {
    return new ObjectCOperation(coperation);
  }
  return new ObjectEmpty();
}

void Object::Print(int space) { std::cout << "[Object]" << std::endl; }

void ObjectEmpty::Print(int space) {
  std::cout << "[ObjectEmpty]" << std::endl;
}

Object* ObjectEmpty::Clone(){
  return new ObjectEmpty();
}


