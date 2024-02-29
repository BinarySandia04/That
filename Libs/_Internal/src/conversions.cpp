#include "conversions.h"

#include <string>

std::string IntToString(int n, std::string s){
  return std::to_string(n);
}

std::string NumToString(double d, std::string s){
  return std::to_string(d);
}
