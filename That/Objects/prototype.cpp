#include "prototype.h"

#include "type.h"

#include <iostream>

using namespace That;

ObjectProtoType::ObjectProtoType(CType *cTypeInfo) {
  this->cTypeInfo = cTypeInfo;
  this->typeMethods = new ObjectContainer(&(cTypeInfo->children));
  this->identifier = cTypeInfo->name;
}

ObjectProtoType::ObjectProtoType(ObjectContainer* typeMethods, std::string identifier){
  this->cTypeInfo = nullptr;
  this->typeMethods = typeMethods;
  this->identifier = identifier;
}

ObjectProtoType::~ObjectProtoType() { delete typeMethods; }

void ObjectProtoType::Print(int n) {
  std::cout << "[ObjectProtoType]" << std::endl;
  this->typeMethods->Print(n + 3);
}


