#include "prototype.h"

#include "type.h"

#include <iostream>

using namespace ZagIR;
using namespace ZagCXX;

ObjectProtoType::ObjectProtoType(CType *cTypeInfo) {
  this->cTypeInfo = cTypeInfo;
  this->typeMethods = new ObjectContainer(&(cTypeInfo->children));
}

ObjectProtoType::ObjectProtoType(ObjectContainer* typeMethods){
  this->typeMethods = typeMethods;
}

ObjectProtoType::~ObjectProtoType() { delete typeMethods; }

void ObjectProtoType::Print(int n) {
  std::cout << "[ObjectProtoType]" << std::endl;
}


