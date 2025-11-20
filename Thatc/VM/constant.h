#pragma once

#include <string>

namespace Thatc {
  enum ConstantType {
    T_INT,
    T_NUMBER,
    T_STRING,
    T_BOL,
    T_NIL
  };

  class Constant {
    public:
    void print();
    ConstantType type;
    union {
      int i;
      double d;
      std::string* s;
      bool b;
    } value;
  };
};
