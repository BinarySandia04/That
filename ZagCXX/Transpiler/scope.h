#pragma once

#include <string>

#include "Objects/object.h"
#include "Objects/container.h"

using namespace ZagIR;

namespace ZagCXX {

class Scope {
public:
  Scope();
  ~Scope();
  Scope(Scope*);
  void Print();

  Object* GetObject(std::string);
  void AddObject(std::string, Object*);
  void AddObject(std::string, Object*, Privacy);
  bool Exists(std::string);
  void Detach();

  auto begin() const { return data->begin(); }
  auto end() const { return data->end(); }
  ObjectContainer* data;
};
};
