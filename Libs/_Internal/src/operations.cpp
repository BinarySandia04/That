#include "operations.h"

#include <string>

std::string _internal::StrPlusInt(std::string s, int n){
  return s + std::to_string(n);
}

std::string _internal::IntPlusStr(int n, std::string s){
  return std::to_string(n) + s;
}

std::string _internal::NumPlusStr(double d, std::string s){
  return std::to_string(d) + s;
}

std::string _internal::StrPlusNum(std::string s, double d){
  return s + std::to_string(d);
}

std::string _internal::StrAccInt(std::string s, int n){
  return std::string(1, s[n]); 
}
