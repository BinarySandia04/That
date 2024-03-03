#pragma once

#include <string>
#include "object.h"

namespace ZagCXX {

class Scope {
public:
  void Print();
  void Delete();
  std::unordered_map<std::string, Object*> data;
};

}; // namespace ZagCXX
