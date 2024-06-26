#pragma once

#include "object.h"

namespace ThatLib {

class Environment;

class ObjectCOperation : public Object {
public:
  std::string GetName();
  ObjectCOperation(COperation *);
  void Print(int);
  Object* Clone();
  COperation *cOperationData;
};

class ObjectConversion : public Object {
public:
  ObjectConversion(Conversion *);
  void Print(int);
  Object* Clone();

private:
  Conversion *conversion;
};

};
