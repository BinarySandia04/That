#include "operation.h"

#include <iostream>

using namespace ThatLib;

ObjectCOperation::ObjectCOperation(COperation *operation) {
  this->cOperationData = operation;
}

void ObjectCOperation::Print(int n) {
  std::cout << "[ObjectCOperation]" << std::endl;
}

Object* ObjectCOperation::Clone(){
  return new ObjectCOperation(cOperationData);
}

std::string ObjectCOperation::GetName() { return this->cOperationData->bind; }

ObjectConversion::ObjectConversion(ThatLib::Conversion *conversion) {
  this->conversion = conversion;
}

void ObjectConversion::Print(int space) {
  std::cout << "[ObjectConversion]" << std::endl;
}

Object* ObjectConversion::Clone(){
  return new ObjectConversion(conversion);
}

