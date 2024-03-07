#include "operations.h"

#include <string>

std::string _internal::StrPlusInt(std::string s, int n){
  return s + std::to_string(n);
}

std::string _internal::IntPlusStr(int n, std::string s){
  return std::to_string(n) + s;
}
